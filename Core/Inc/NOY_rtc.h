#define ISR_IN_BIT 7
#define ISR_INITF_BIT 6
#define RTC_PRER_ASYNC_LSB 16
#define RTC_PRER_SYNC_MSB 14
#define RTC_PRER_SYNC_MASK 0x7FFF
#define RTC_PRER_ASYNC_MASK 0x7F
#define RTC_CR_FMT_MASK 6

void rtc_init_configure(void);
int rtc_check_init_success(void);
int rtc_init(void);
unsigned char decimal_to_bcd(unsigned char decimal);
int rtc_init_date(uint8_t month_day, uint8_t month, uint8_t year, uint8_t seconds, uint8_t minutes, uint8_t hour);
int rtc_init_struct(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);
void get_date(RTC_DateTypeDef *date);
