#include "pwm.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	char *ptr;
	int fd, mmap_size, hz, duty;
	int prescalar, period;

	if(geteuid()) {
		printf("%s\n", "Need to be root");
		return 1;
	}
	
	if(argc < 3)
		return 1;
	hz = atoi(argv[1]);
	duty = atoi(argv[2]);

	if(hz > 400 || hz <= 195 || duty > 100 || duty < 0)
		return 1;
	if(hz < 261){
		prescalar = 4; //div 480
		period = 50*1000/hz;
	}else if(hz < 392){
		prescalar = 3; //div 360
		period = 66666/hz;
	}

	fd = open("/dev/mem", O_RDWR);
	if(fd < 0){
		perror("open");
		return 1;
	}

	mmap_size = (CONFIG_REG_END_ADDR - CONFIG_REG_START_ADDR);
	ptr = (char*)mmap(0, mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, CONFIG_REG_START_ADDR);

	if(ptr == MAP_FAILED){
		perror("mmap");
		return 1;
	}

	//清空PI3寄存器，将PI3接到PWM引脚上
	ACCESS_REG(ptr, PI_CFG0) &= ~PI3_CFG_MASK;
	ACCESS_REG(ptr, PI_CFG0) |= PI3_CFG_PWM1;

	//确定PWM的频率和占空比，写入PWM_CH1_PERIOD寄存器中
	ACCESS_REG(ptr, PWM_CH1_PERIOD) = 
		PWM_CH1_ACT_CYC((int)((double)duty / 100 * period)) | PWM_CH1_ENTIRE_CYC(period - 1);

	//清空PWM_CH1各个位
	ACCESS_REG(ptr, PWM_CTRL) &= ~PWM_CH1_MASK;
	
	//设定PWM预分频，输出模式为方波
	ACCESS_REG(ptr, PWM_CTRL) |= 
		PWM_CH1_PRESCALAR(prescalar) | PWM_CH1_EN(1) |
		PWM_CH1_ACT_STATE(1) | PWM_CH1_MODE(0);

	//打开PWM使能位
	ACCESS_REG(ptr, PWM_CTRL) |= PWM_CH1_CLK_GATING(1);

	munmap(ptr, mmap_size);
	close(fd);

	return 0;
}
