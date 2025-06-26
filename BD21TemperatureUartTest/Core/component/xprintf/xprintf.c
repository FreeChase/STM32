#include "xprintf.h"
#include "usart.h"


typedef struct {
    int width;          // 字段宽度
    bool left_align;    // 左对齐标志
    char pad_char;      // 填充字符
    bool is_uppercase;  // 大写标志
    int min_digits;     // 最小位数
    int precision;      // 浮点数精度
} format_info;

// 辅助函数：向缓冲区写入字符
void my_putchar(char c) {
    //XXX 实际实现时替换为您的输出函数
    // putchar(c);
    HAL_UART_Transmit(&huart6, (uint8_t*)&c, 1, HAL_MAX_DELAY);
}

void my_puts(const char *s) {
    while (*s) {
        my_putchar(*s++);
    }
}

// 辅助函数：获取整数部分的位数
int get_num_width(uint64_t num, int base) {
    int width = 0;
    do {
        width++;
        num /= base;
    } while (num > 0);
    return width;
}

// 辅助函数：填充字符
void pad_space(char pad_char, int count) {
    while (count-- > 0) {
        my_putchar(pad_char);
    }
}

// 辅助函数：打印十进制数
void print_decimal(int64_t num, format_info *info) {
    char dec[24];  // 足够存放64位整数的字符数组
    int i = 0;
    bool is_negative = false;
    
    // 处理0的特殊情况
    if (num == 0) {
        int zeros = info->min_digits ? info->min_digits : 1;
        while (zeros--) my_putchar('0');
        return;
    }
    
    // 处理负数
    if (num < 0) {
        is_negative = true;
        num = -num;  // 转为正数处理
    }
    
    // 转换为字符串
    while (num) {
        dec[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // 补足最小位数
    while (i < info->min_digits) dec[i++] = '0';
    
    // 计算实际输出宽度
    int total_width = i + (is_negative ? 1 : 0);
    
    // 右对齐时先输出填充
    if (!info->left_align && info->width > total_width) {
        if (info->pad_char == '0' && is_negative) {
            my_putchar('-');  // 对于0填充，负号要在前面
            is_negative = false;  // 防止重复输出负号
        }
        pad_space(info->pad_char, info->width - total_width);
    }
    
    // 输出负号（如果还没输出）
    if (is_negative) my_putchar('-');
    
    // 输出数字
    while (i--) my_putchar(dec[i]);
    
    // 左对齐时后面补充填充
    if (info->left_align && info->width > total_width) {
        pad_space(info->pad_char, info->width - total_width);
    }
}

// 辅助函数：处理浮点数
void print_float(double num, int precision) {
    // 处理负数
    if (num < 0) {
        my_putchar('-');
        num = -num;
    }
    
    // 处理整数部分
    uint64_t int_part = (uint64_t)num;
    double frac_part = num - int_part;
    
    // 打印整数部分
    if (int_part == 0) {
        my_putchar('0');
    } else {
        char int_buffer[20];
        int i = 0;
        while (int_part > 0) {
            int_buffer[i++] = '0' + (int_part % 10);
            int_part /= 10;
        }
        while (i > 0) {
            my_putchar(int_buffer[--i]);
        }
    }
    
    // 如果指定了精度，打印小数部分
    if (precision > 0) {
        my_putchar('.');
        
        // 处理小数部分
        while (precision > 0) {
            frac_part *= 10;
            int digit = (int)frac_part;
            my_putchar('0' + digit);
            frac_part -= digit;
            precision--;
        }
    }
}

// 辅助函数：输出十六进制数
void print_hex(uint64_t num, int min_digits, bool uppercase) {
    char hex[16];
    int i = 0;
    
    if (num == 0) {
        int zeros = min_digits ? min_digits : 1;
        while (zeros--) my_putchar('0');
        return;
    }
    
    while (num) {
        int digit = num & 0xF;
        hex[i++] = digit < 10 ? '0' + digit : 
                   (uppercase ? 'A' : 'a') + (digit - 10);
        num >>= 4;
    }
    
    while (i < min_digits) hex[i++] = '0';
    
    while (i--) my_putchar(hex[i]);
}

// 辅助函数：解析格式化信息
format_info parse_format(const char **format) {
    format_info info = {0, false, ' ', false, 0, 6};  // 默认精度为6
    
    // 检查左对齐标志
    if (**format == '-') {
        info.left_align = true;
        (*format)++;
    }
    
    // 检查填充字符
    if (**format == '0') {
        info.pad_char = '0';
        (*format)++;
    }
    
    // 解析宽度
    while (**format >= '0' && **format <= '9') {
        info.width = info.width * 10 + (**format - '0');
        (*format)++;
    }
    
    // 解析精度
    if (**format == '.') {
        (*format)++;
        info.precision = 0;
        while (**format >= '0' && **format <= '9') {
            info.precision = info.precision * 10 + (**format - '0');
            (*format)++;
        }
    }
    
    return info;
}

void xprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++; // 跳过%
            
            // 解析格式化信息
            format_info info = parse_format(&format);
            
            switch (*format) {
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    print_decimal(num, &info);
                    break;
                }
                
                case 'f': {
                    double num = va_arg(args, double);
                    // 计算整个数字的长度（包括小数点和小数部分）
                    char temp_buf[32];
                    int num_len = snprintf(temp_buf, sizeof(temp_buf), "%.*f", 
                                         info.precision, num);
                    
                    if (!info.left_align) {
                        pad_space(info.pad_char, info.width - num_len);
                    }
                    print_float(num, info.precision);
                    if (info.left_align) {
                        pad_space(info.pad_char, info.width - num_len);
                    }
                    break;
                }
                
                case 's': {
                    char *str = va_arg(args, char *);
                    int len = 0;
                    char *p = str;
                    while (*p++) len++;
                    
                    if (!info.left_align) {
                        pad_space(info.pad_char, info.width - len);
                    }
                    my_puts(str);
                    if (info.left_align) {
                        pad_space(info.pad_char, info.width - len);
                    }
                    break;
                }
                
                case 'p': {
                    void *ptr = va_arg(args, void *);
                    my_putchar('0');
                    my_putchar('x');
                    print_hex((uint64_t)ptr, sizeof(void*) * 2, true);
                    break;
                }
                
                case 'X':
                    info.is_uppercase = true;
                    // fall through
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    if (info.pad_char == '0' && info.width > 0) {
                        print_hex(num, info.width, info.is_uppercase);
                    } else {
                        int num_width = get_num_width(num, 16);
                        if (!info.left_align && info.width > num_width) {
                            pad_space(info.pad_char, info.width - num_width);
                        }
                        print_hex(num, 0, info.is_uppercase);
                        if (info.left_align && info.width > num_width) {
                            pad_space(info.pad_char, info.width - num_width);
                        }
                    }
                    break;
                }
                
                case 'c': {
                    char c = (char)va_arg(args, int);
                    my_putchar(c);
                    break;
                }
                
                case 'l': {
                    format++;
                    if (*format == 'd' || *format == 'i') {
                        long num = va_arg(args, long);
                        print_decimal(num, &info);
                    } else if (*format == 'l' && (*(format + 1) == 'd' || *(format + 1) == 'i')) {
                        format++;
                        int64_t num = va_arg(args, int64_t);
                        print_decimal(num, &info);
                    } else if (*format == 'l' && *(format + 1) == 'X') {
                        format++;
                        uint64_t num = va_arg(args, uint64_t);
                        print_hex(num, 16, true);
                    }
                    break;
                }
                
                default:
                    my_putchar('%');
                    my_putchar(*format);
                    break;
            }
        } else {
            my_putchar(*format);
        }
        format++;
    }
    
    va_end(args);
}
