#ifndef _COMMMON_H_
#define _COMMMON_H_

#define MASK_COLOR		0xF0 	//花色掩码
#define MASK_VALUE		0x0F 	//数值掩码

#define NT_COLOR 		0x40	//主牌花色
#define KING_SMALL 		0x41  	//小王
#define KING_BIG		0x42	//大王
#define ERROR_COLOR		0xF0 	//错误花色

#define TOTAL_CARD_CNT	108		//扑克牌数量
#define TOTAL_CARD_CNT34  92    //去掉3，4的扑克牌数量
#define PLAYER_CARD_CNT	25		//玩家手牌数量
#define PLAYER_CARD_CNT34 21    //去掉34玩家手牌
#define COLOR_CARD_CNT	15		//玩家手牌数量
#define BACK_CARD_CNT	8		//底牌数量
#define GAME_PLAYER		4		//玩家人数

#define MAX_INDEX		54

#define TYPE_ERROR				0 		//错误类型
#define TYPE_SINGLE				1 		//单张
#define TYPE_DOUBLE				2 		//对子
#define TYPE_TRACKER			3 		//拖拉机
#define TYPE_FLING				4 		//甩牌
#define TYPE_FLING_DB			5 		//甩牌

#define MIN_NTC_WEIGHT			40		//主牌的最小权值
#define MIN_NTC_WEIGHT_N		52		//无主时，主牌的最小权值

#define INVALID_CHAIR  				GAME_PLAYER							//无效座位号
#define INVALID_CARD				0xFF								//无效牌

typedef unsigned char 		_uint8;
typedef unsigned short		_uint16;
typedef unsigned int		_uint32;
typedef int					_tint32;

//玩法ID
enum PlayerID
{
	GAME_PLAYERS_3							= 1109001,			//游戏人数3
	GAME_PLAYERS_4							= 1109002,			//游戏人数4

	PAISHUDEL3_4  					    	= 1001001,			//去掉3和4
	PAISHUNDEL3_4						    = 1001002,			//不去掉3和4

	NTNUMN5                                 = 1002001,          //不打5，10，K
	NTNUM5                                  = 1002002,          //打5，10，k

	NTNUMN2                                 =1003002,           //2不是常主
	NTNUM2                                  =1003001,           //2是常主
};




//没用
//牌型分析结构
struct tagCardAnalyse
{
	_uint8 nSingleCnt;						//单牌数量
	_uint8 nDoubleCnt;						//对子数量
	_uint8 nTrackorCnt;						//拖拉机数量
	_uint8 nCardColor;						//牌的花色

	_uint8 nSmallSingleCard;				//最小单牌
	_uint8 nBigSingleCard;					//最大单牌
	_uint8 cbDoubleCards[12];				//对子牌
	_uint8 cbTrackorCards[36];				//拖拉机牌 （0 分割，长度，拖拉机数据）
};

#endif