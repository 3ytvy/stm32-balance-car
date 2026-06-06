#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "Delay.h"

//函数：对PID控制器的系数进行初始化
void PID_Init(PID_TypeDef *PID)
{	
	PID->err_k=0; 
	PID->err_k_1=0.0f;
	PID->err_int=0.0f;
	
	PID->Out=0;
	PID->Actual=0;
	PID->Target=0;
	
	//初始化Ua上下限为-∞到+∞（3.4e+38f为float类型最大值）
	PID->UpperLimit=3.4e+38f;
	PID->LowerLimit=-3.4e+38f;
} 

//函数：改变pid设定值
void PID_ChangeTarget(PID_TypeDef *PID,float Target)
{

	PID->Target = Target;
}


//函数：设置Ua（PID的输出）上下限的编程接口
void PID_LimitConfig(PID_TypeDef *PID,float Upper,float Lower)
{
	PID->LowerLimit = Lower;
	PID->UpperLimit = Upper;
}


//函数：PID复位函数
//每次开关PID，都要对PID控制器存储在结构体的部分参数进行复位
void PID_Reset(PID_TypeDef *p)
{
	p->err_int=0;
	p->err_k=0;
	p->err_k_1=0;
}



//
//@	函数：PID计算及结构体变量值更新
//	参数：p 指定结构体的地址
//
void PID_Update(PID_TypeDef *p)
{
	//获取本次误差和更新上次误差
	p->err_k_1 = p->err_k;//更新上次误差
	p->err_k = p->Target - p->Actual;//获取本次误差：目标值 - 实际值
	
	//外环误差积分（累加）
	//如果Ki不为零，才进行误差积分，防止Ki为0而纯积分，会产生严重振荡
	if(p->Ki != 0)//如果Ki不为0
	{
		p->err_int += p->err_k;//进行误差积分
	}
	else
	{
		p->err_int=0;
	}
	
	//PID计算
	p->Out = p->Kp * p->err_k
					+ p->Ki * p->err_int
					- p->Kd * (p->Actual - p->Actual1);//微分现行
	
	//输出偏移（输出非零值时候的一个小偏移）
	if(p->Out > 0){p->Out += p->outoffset;}
	if(p->Out < 0){p->Out -= p->outoffset;}
	
	//输出限幅
	if(p->Out > p->UpperLimit){p->Out = p->UpperLimit;}
	if(p->Out < p->LowerLimit){p->Out = p->LowerLimit;}
	
	//更新“上次的实际值”
	p->Actual1 = p->Actual;

}



////函数：执行一次PID运算
////参数：FB - 传感器采回的反馈的值
////返回值：PID控制器计算的结果CO，等效于Ua
//float PID_Compute(PID_TypeDef *PID,float FB)
//{
//	float err = PID->SP - FB;//误差err = 设定值SP - 反馈值FB
//	
//	float COp = PID->Kp * err;

//	//通过离散的积分和离散的微分的方法来算误差err的积分项和微分项的值
//	uint64_t t_k=GetUs(); //t_k等于获取当前时间值
//	
//	float deltaT=(t_k - PID->t_k_1)*1.0e-6f;//t_k减去t_k-1就是△T，所得结果单位是微秒，转为单位秒，要×1.0e-6f
//	
//	float err_dev = 0.0f;
//	float err_int = 0.0f;
//		
//	if(PID->t_k_1 != 0)//判断是否为首次运算
//	{
//		err_dev = (err - PID->err_k_1) / deltaT;//当前误差值err减去上次的误差值err_k_1所得的差除以△T，就是误差err的微分项
//		err_int = PID->err_int_k_1 + (err + PID->err_k_1) * deltaT * 0.5f;//当前误差值err加上 通过图形所得(err + PID->err_k_1) * deltaT * 1/2 就是新增的小块面积，就是误差err的积分项
//	}
//	
//	float COd = PID->Kd * err_dev;//COd = 微分项系数 * 误差err的微分项
//	float COi = PID->Ki * err_int;//COi = 积分项系数 * 误差err的积分项
//	
////	//对COi（积分项）进行限幅
////	if(COi>PID->UpperLimit)
////	{COi=PID->UpperLimit;}
////	if(COi<PID->LowerLimit)
////	{COi=PID->LowerLimit;}
//	
//	float CO = COp + COd + COi;
//	
//		//更新结构体PID里的“上一次的时间、误差err的值、误差err的积分值”
//	PID->t_k_1 = t_k;
//	PID->err_k_1 = err;
//	PID->err_int_k_1 = err_int;

////	//对CO（亦即Ua）进行限幅
////	if(CO>PID->UpperLimit)
////	{CO=PID->UpperLimit;}
////	if(CO<PID->LowerLimit)
////	{CO=PID->LowerLimit;}
//	
//	return CO;
//}

// 这是【你的原版逻辑】+【仅2处安全改进】
// 微分、积分、公式、结构 完全和你原来一样！
//float PID_Compute(PID_TypeDef *PID,float FB)
//{
//	float err = PID->SP - FB;
//	
//	float COp = PID->Kp * err;

//	uint64_t t_k=GetUs();
//	
//	float deltaT=(t_k - PID->t_k_1)*1.0e-6f;
//    
//    // ====================== 【唯一安全改进 1】 ======================
//    // 防止 deltaT=0 导致 除零错误死机，完全不影响你的公式和计算
//    if(deltaT <= 0.0f) deltaT = 0.00001f; 
//	
//	float err_dev = 0.0f;
//	float err_int = 0.0f;
//		
//	if(PID->t_k_1 != 0)
//	{
//		err_dev = (err - PID->err_k_1) / deltaT;  // 你的原微分 完全不动
//		err_int = PID->err_int_k_1 + (err + PID->err_k_1) * deltaT * 0.5f;  // 你的原积分 完全不动
//	}
//	
//	float COd = PID->Kd * err_dev;
//	float COi = PID->Ki * err_int;
//	
//	//对COi（积分项）进行限幅
//	if(COi>PID->UpperLimit)
//	{COi=PID->UpperLimit;}
//	if(COi<PID->LowerLimit)
//	{COi=PID->LowerLimit;}
//	
//	float CO = COp + COd + COi;
//	
//	PID->t_k_1 = t_k;
//	PID->err_k_1 = err;
//	PID->err_int_k_1 = err_int;
//	
//	//对CO（亦即Ua）进行限幅
//	if(CO>PID->UpperLimit)
//	{CO=PID->UpperLimit;}
//	if(CO<PID->LowerLimit)
//	{CO=PID->LowerLimit;}
//	
//	return CO;
//}












