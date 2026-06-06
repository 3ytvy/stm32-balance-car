#include "stm32f10x.h"                  // Device header

//ADC初始化（这里初始化ADC1，让其模式是：单次转换非扫描模式，并且用通道6即ADC_Channel_6，其对应引脚是PA6）
void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//打开ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//打开GPIOA的时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);;//给ADC1输入时钟，用的是RCC的APB2的六分频
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//引脚模式是模拟输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_55Cycles5);
	//注意这个函数名称，ADC_RegularChannelConfig的Regular是规则组的意思
	//意即这里是在配置规则组的序列了
	//如果是注入组（不常用），则是ADC_InjectedChannelConfig
						//		void ADC_RegularChannelConfig(
						//    ADC_TypeDef* ADCx,          // 选择哪个ADC（ADC1/ADC2/ADC3）
						//    uint8_t ADC_Channel,        // 选择采集哪个通道（引脚）
						//    uint8_t Rank,               // 放在序列几
						//    uint8_t ADC_SampleTime      // 采样时间（越长越精准） 这里无关紧要
						//也就是说这个是用来把 指定通道 放到 指定序列 的函数，而且只放一个
	
	//单次转换 非扫描 模式
	//下面这些是在配置规则组
	//而注入组的配置有它自己独立的配置
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立模式，其他模式不用管
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//数据右对齐，直接读取
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//不适用硬件触发，相当于是说就是使用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//不连续转换（单次转换模式，每完成一次转换就结束，下一次转换需要再次触发）
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//不扫描（非扫描模式，每次转换就只管第一个序列的通道）
	ADC_InitStructure.ADC_NbrOfChannel=1;//通道数目
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//给ADC上电
	ADC_Cmd(ADC1,ENABLE);
	
	//下面这个是校准的，不用太深入
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)==SET);
	
//	ADC_SoftwareStartConvCmd(ADC1,ENABLE); //如果是连续转换，在初始化完触发一下即可
}



//获取ADC转换后的数据
uint16_t AD_GetValue(void)
{
	//注意如果是连续转换的话，这个软件触发挪到AD初始化那里就行了，因为连续转换只要触发一次
	//因为连续转换的话，数据寄存器会不断刷新转换结果（新的覆盖老的）
	//所以不用等待转换完成的标志位EOC，直接return即可
	
	//与单次转换的区别：单次转换只会转换一次，我们通过判断EOC是否被置一来判断是否转换完成
	//然后读取数据的时候EOC会自动清零
	

	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//这就是软件触发，执行这句代码就是软件触发ADC1
	
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//等待ADC_FLAG_EOC为1跳出这个空循环，此时EOC标志位为1表示转换完成
	
	return ADC_GetConversionValue(ADC1);//获取数据寄存器里的最后数据，执行完这个函数，EOC标志位会被自动清零
}





