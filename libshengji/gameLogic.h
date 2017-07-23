#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common.h"

static const _uint8 g_cbCards[54] = {
//  0     1     2     3     4     5     6     7     8     9     10    11    12		--
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 A2--K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 A2--K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 A2--K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 A2--K
	0x41, 0x42																	  //小王大王
};
/*
	12 	  52     1     2     3     4     5     6     7     8     9    10    11 
	25 	  52    14    15    16    17    18    19    20    21    22    23    24 
	38 	  52    27    28    29    30    31    32    33    34    35    36    37 
	51 	  53    40    41    42    43    44    45    46    47    48    49    50 --主级牌是0x32 , 黑桃2
	54 	  55    

	   NOTE：
   	1、牌对应的权重，一张牌一个权重（与上面的牌值定义对应），用来对牌进行排序，或者牌型判断
	2、权重相差为1的对子为拖拉机，同一花色权重越大，牌越大，eg，大王大王，小王小王，对黑桃2，对红桃2，对黑桃A
	3、有主时，最小的主牌权重是40， 无主时，最小的主牌权重是52，且所有级牌都是52，小王53，大王54
*/

/*--------------游戏主逻辑类 用于2副牌升级打法------------*/

struct tagCard
{
	_uint8 card;
	_uint8 ntCard;
	bool bNTC; //是否无主
	bool NTCNum2;  //2是否常主
	bool CardDel34; //是否删除34
	tagCard()
	{
		card = INVALID_CARD;
		ntCard = INVALID_CARD;
		bNTC = true;
		NTCNum2 = false;
		CardDel34 = false;
	}
};

struct tagCardsGroup
{
};

class CGameLogic
{
  private:
	_uint8 m_nNTNum;   //主牌数字
	_uint8 m_nNTColor; //主牌花色
	_uint8 m_nNTCCard; //主牌花色
	bool m_bNTC;	   //是否无主
	bool m_bNtcNum2;    //2是否常主
	bool m_bDel34;      //是否删除34

  public:
	CGameLogic();
	/************************没用*****************************
	//提取对子--cbCards是有序的
	_uint8 TakeOutDouble(_uint8 cbCards[], _uint8 nCount, _uint8 cbOutCards[], bool bRemove);
	//提取拖拉机--cbCards是有序的
	_uint8 TakeOutTracker(_uint8 cbCards[], _uint8 nCount, _uint8 cbOutCards[], bool bRemove);
	//获取最长的拖拉机--cbCards是有序的
	_uint8 GetMaxLongTracker(_uint8 cbCards[], _uint8 nCount, _uint8 cbOutCards[]);
	//提取同一花色的所有牌--cbCards是有序的 同花色在一个区间内
	_uint8 TakeOutCardByColor(_uint8 cbCards[], _uint8 nCount, _uint8 nColor, _uint8 cbOutCards[], bool bRemove);

public:
	//分析牌
	bool AnalyseCard(_uint8 cbCards[], _uint8 nCount, tagCardAnalyse &tCardAna);
	//自动出牌逻辑
	bool AutoOutCard(_uint8 cbHandCards[], _uint8 nHandCnt, _uint8 cbBaseCards[], _uint8 nBaseCnt,
							 _uint8 cbResultCards[], _uint8 nResultCnt);
	************************没用*****************************/

  public:
	/*****************************************************************************
	*
	*         ***      **   *********   ***********  ***********
	*         ****     **   **     **       ***      **
	*         *****    **   **     **       ***      **
	*         **  **   **   **     **       ***      ***********
	*         **    ** **   **     **       ***      **
	*         **     ****   **     **       ***      **
	*         **      ***   *********       ***      ***********
	*
	* note : 	1.下面基本所有函数都需要保证 手牌/组合牌 是排好序的，
	*   		没有排序需要调用SortCard函数进行排序，才能得到正确结果
	*
	* 			2.涉及比较牌的时候，先出的牌在参数的位置靠前，因为牌一样，先出的人牌大
	*			eg, Compare(first, second), first为第一个玩家的出牌
	*
	*******************************************************************************/
	//从大到小排序
	void SortCard(_uint8 cbCards[], _uint8 nCount);
	//
	//static _uint8 GetWeight(const _uint8 card, const _uint8 ntCard_);
	static _tint32 GetSortWeightOld(bool CardDel34, bool NTCNum2, const _uint8 card_, const _uint8 ntCard_, const bool bNTC, const _uint8 sort);
	static _tint32 GetSortWeight(const _uint8 card_, const _uint8 ntCard_, const bool bNTC, const _uint8 sort = 0, const bool isNt2 = false, const bool bDel34 = false);
	_tint32 GetWeight(const _uint8 card);
	static _tint32 GetSortWeight(const tagCard &card);


	//获取牌面得分
	int GetScore(_uint8 nCard);
	//获取一轮得分 返回得分牌明细和得分
	int GetScore(const _uint8 cbCards[], _uint8 nCount, _uint8 cbScoreCards[], _uint8 &nScoreCnt);
	//获取底牌得分
	int GetBackScore(const _uint8 cbBackCards[], _uint8 nBackCnt);

	//设置主牌花色
	//inline void SetNTColor(_uint8 nCard){ m_nNTColor =  nCard & MASK_COLOR; };
	inline void SetNTCard(_uint8 card, bool bNTC = true) { m_bNTC = bNTC; m_nNTCCard = card; m_nNTColor = GetCardColor(card); m_nNTNum = GetCardNum(card); };
	inline void SetNTNum2(bool m_Num2) { m_bNtcNum2 = m_Num2; }
	inline void SetDel34(bool del34) { m_bDel34 = del34; };
	inline _uint8 GetNTCard() { return m_nNTCCard; };
	//获取2是否常主
	inline bool GetNTNum2() { return m_bNtcNum2; };
	inline bool GetDel34() { return m_bDel34; };
	//inline void SetNTColor(_uint8 color) { /*m_nNTColor = color;*/ };
	//获取主牌花色
	inline _uint8 GetNTColor() { return m_nNTColor; };
	//设置主牌数值
	//inline void SetNTNum(_uint8 nCard) { /*m_nNTNum = nCard & MASK_VALUE;*/ };
	//获取主牌数值
	inline _uint8 GetNTNum() { return m_nNTNum; };
	//获取主牌数值
	inline bool GetbNTC() { return m_bNTC; };

	static _uint8 SwitchToCardData(_uint8 cbCardIndex);
	static _uint8 SwitchToCardIndex(_uint8 cbCardData);
	//获取牌的数值 16代表A
	static inline _uint8 GetCardNum(_uint8 nCard) { return (nCard & MASK_VALUE); };
	//洗牌
	void RandCard(_uint8 cbCards[]);
	//有效判断
	static bool IsValidCard(_uint8 cbCardData);
	static _uint8 GetCardColor(_uint8 nCard);
	_uint8 GetCardCnt(_uint8 Card, const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt);
	//牌的相对大小
	static int CompareCard(const void *nCard1, const void *nCard2);

	//是否可以甩牌 必须是同花色 甩的牌均为最大牌 甩牌失败 强制出(cbResultCards)最小的牌
	bool CanFlingCard(const _uint8 cbOutCards[], _uint8 nOutCnt, _tint32 nOutChair,
					  const _uint8 cbOtherCards[][PLAYER_CARD_CNT], _uint8 cbGamePlayer, const _uint8 nCardCnt[],
					  _uint8 cbResultCards[], _uint8 &nResultCnt, _uint8 &type);

	//从手牌中删除牌
	_uint8 RemoveCard(_uint8 cbRemoveCards[], _uint8 cRemoveCnt, _uint8 cbHandCards[], _uint8 &cHandCnt);
	//比较两个玩家的出牌，比较大小
	/*
	* 比较牌的时候，先出的牌在参数的位置靠前，因为牌一样，先出的人牌大
	* 比如 cbOutCards_Lps 为先出的牌，cbROutCards_Rps为跟牌玩家出的牌
	*/
	bool CompareOutCard(const _uint8 cbOutCards_Lps[], const _uint8 cbROutCards_Rps[], _uint8 nOutCnt);

	/*********************/
	/*
	* _uint8 outCards[], 	出的牌
	* _uint8 outCardsCnt ,	出的牌数量
	* _uint8 cards[], 		手牌
	* _uint8 cardsCnt ,		手牌数量
	* _uint8 mustOutCard[],	必须出的牌
	* _uint8& mustOutCnt,	必须出的牌的数量
	* _uint8 canOutCard[], 	能出的牌
	* _uint8& canOutCnt, 	能出的牌的数量
	* _uint8 canOutClr[], 	能出的花色
	* _uint8& canOutClrCnt	能出的花色数量
	*/
	_uint8 GetOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
					   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);

	//判断跟的牌是否合法
	bool CanOutCard(const _uint8 cbOutCards[], _uint8 nOutCnt, const _uint8 cbFirstCards[], _uint8 nFirstCardsCnt,
					const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt);

	//获取牌型
	_uint8 GetCardType(const _uint8 cbCards[], _uint8 nCount);

	//获取底牌翻倍
	_uint8 GetRepFan(_uint8 cbRepCards[], _uint8 nCount);

  private:
	//判断两张牌花色一致
	bool SameClr(_uint8 cbCard1, _uint8 cbCard2)
	{
		return ((NTC(cbCard1) && NTC(cbCard2)) || (GetCardColor(cbCard1) == GetCardColor(cbCard2) && (!NTC(cbCard1) && !NTC(cbCard2))));
	}
	//判断一张牌花色是不是主牌
	bool NTC(_uint8 cbCard)
	{
		return (((m_bNTC) && GetCardColor(cbCard) == m_nNTColor) || SpNTC(cbCard));
	}

	//是不是特殊的主
	bool SpNTC(_uint8 cbCard)
	{
		return (cbCard == 0x41 || cbCard == 0x42 || IsNTNum(cbCard) || (m_bNtcNum2 && (GetCardNum(cbCard) == 2)));
	}

	bool IsNTNum(_uint8 cbCard)
	{
		return (GetCardNum(cbCard) == m_nNTNum);
	}

	//判断一列牌是否是同一个花色
	bool GetCardListColor(const _uint8 cbCards[], _uint8 nCount);
	//是否是单牌
	inline bool IsSingle(const _uint8 cbCards[], _uint8 nCount) { return nCount == 1; };
	//是否是对子
	inline bool IsDouble(const _uint8 cbCards[], _uint8 nCount){ if (nCount != 2) 	{ return false; } return cbCards[0] == cbCards[1]; };
	//
	_uint8 SwitchCardToUser(_uint8 cbCardData[], _uint8 bCardCount, _uint8 cbCardIndex[MAX_INDEX]);

	//是否是甩牌,返回甩牌类型
	_uint8 IsFling(const _uint8 cbCards[], _uint8 nCount);
	//清理数量为0的牌
	_uint8 RemoveNoCard(_uint8 cbHandCards[], _uint8 cHandCnt);
	//是否是拖拉机
	bool IsTracker(const _uint8 cbCards[], _uint8 nCount);
	//是否满足拖拉机规则
	bool IsTrackerRule(_uint8 nCardFirst, _uint8 nCardNext);

	bool GetTracker(const _uint8 db[], _uint8 dbCnt, _uint8 start[], _uint8 length[], _uint8& cnt );

	bool CompareFlingCard(const _uint8 cbOutCards_Lps[], const _uint8 cbROutCards_Rps[], _uint8 nOutCnt);
	//单张甩牌，最小牌和其他人的牌比较，
	bool CompareFlingSGCard(_uint8 flingCard, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt);

	//对子甩牌
	bool CompareFlingDBCard(const _uint8 db[], _uint8 dbCnt , _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt, _uint8 cbResultCards[], _uint8 &nResultCnt);
	//单张甩牌，最小牌和其他人的牌比较，
	bool CompareFlingSGCardOld(_uint8 flingCard, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt);

	//对子甩牌
	bool CompareFlingDBCardOld(const _uint8 db[], _uint8 dbCnt , _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt, _uint8 cbResultCards[], _uint8 &nResultCnt);

	//获取同花色的牌还有多少张
	_uint8 GetSameColorNum(const _uint8 cbCards[], _uint8 nCount, _uint8 card);
	/* cbCard1 > cbCard2 ?
	*  花色一致，或者cbCard2为主花色， 那么 GetWeight(cbCard1) < GetWeight(cbCard2) 则 cbCard1 < cbCard2, 否则cbCard1 > cbCard2（不同花色，且都不是主花色，先出的人大）
	*  牌一样，先出的人大
	*/
	bool Compare(_uint8 cbCard1, _uint8 cbCard2) { return (!((SameClr(cbCard1, cbCard2) || NTC(cbCard2)) && (GetWeight(cbCard1) < GetWeight(cbCard2)))); }

	//单张出牌
	bool GetSGOutCards(_uint8 outCard, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[], _uint8 &mustOutCnt,
					   _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);
	//对子出牌
	bool GetDBOutCards(_uint8 outCard, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[], _uint8 &mustOutCnt,
					   _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);
	//拖拉机
	bool GetTKOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
					   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);
					   
	bool GetFLOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt, \
						_uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);
	//对子甩牌
	bool GetFDOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
					   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);
	//单张甩牌
	bool GetFSOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
					   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt);

	_uint8 GetAllDBByClr(const _uint8 cards[], _uint8 cardsCnt, _uint8 sg[], _uint8* sgCnt, _uint8 db[], _uint8* dbCnt, _uint8 mdb[], _uint8* mdbCnt, _uint8 card);
	/*********************/

	bool judgeMust(const _uint8 mustOutCard[], _uint8 mustOutCnt,  _uint8 cbOutCards[], _uint8 nOutCnt);

	bool judgeCan(const _uint8 canOutCard[], _uint8 canOutCnt,  _uint8 cbOutCards[], _uint8 nOutCnt, const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt);

	bool judgeClr(const _uint8 canOutClr[], _uint8 canOutClrCnt,  _uint8 cbOutCards[], _uint8 nOutCnt);

};

#endif