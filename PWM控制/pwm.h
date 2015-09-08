//参考 http://linux-sunxi.org/PWM_Controller

#define CONFIG_REG_START_ADDR	0x01c00000
#define CONFIG_REG_END_ADDR		0x01c2c400
// #define PWM_BASE_ADDR  0x01c20000 //0x01c20c00
// #define PWM_MAP_SIZE   4096

#define ACCESS_REG(x,y) (*(volatile unsigned int*)((x)+((y)-CONFIG_REG_START_ADDR)))

#define PI_CFG0			0x01c20920
#define PI3_CFG_MASK	(7<<12)
#define PI3_CFG_PWM1	(2<<12)

#define PWM_CTRL       0x01c20e00 //0xe00
#define PWM_CH0_PERIOD 0x01c20e04 //0xe04
#define PWM_CH1_PERIOD 0x01c20e08 //0xe08

#define	PWM_CH0_PRESCALAR(x)	(((x)&7)<<0)
#define	PWM_CH0_EN(x)			(((x)&1)<<4)
#define	PWM_CH0_CLK_GATE(x)		(((x)&1)<<5)
#define	PWM_CH0_PULSE_START(x)	(((x)&1)<<6)
#define	PWM_CH0_MODE(x)			(((x)&1)<<7)
#define	PWM_CH0_PUL_START(x)	(((x)&1)<<8)
#define	PWM_CH0_MASK			(0x1FF)

#define	PWM_CH1_PRESCALAR(x)	(((x)&7)<<15)
#define	PWM_CH1_EN(x)			(((x)&1)<<19)
#define	PWM_CH1_ACT_STATE(x)	(((x)&1)<<20)
#define	PWM_CH1_CLK_GATING(x)	(((x)&1)<<21)
#define	PWM_CH1_MODE(x)			(((x)&1)<<22)
#define	PWM_CH1_PUL_START(x)	(((x)&1)<<23)
#define	PWM_CH1_MASK			(0x1FF<<15)

#define	PWM_CH0_ACT_CYC(x)		(((x)&0xFFFF)<<0)
#define	PWM_CH0_ENTIRE_CYC(x)	(((x)&0xFFFF)<<16)
#define	PWM_CH1_ACT_CYC(x)		(((x)&0xFFFF)<<0)
#define	PWM_CH1_ENTIRE_CYC(x)	(((x)&0xFFFF)<<16)

