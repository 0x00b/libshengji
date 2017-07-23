#include "gameLogic.h"

CGameLogic::CGameLogic()
{
	m_nNTNum = 2;
	m_nNTColor = 3;
	m_nNTCCard = 0x32; //主牌
	m_bNTC = true;	 //是否无主 true:有主，false:无主
}

//获取牌面得分
int CGameLogic::GetScore(_uint8 nCard)
{
	_uint8 val = GetCardNum(nCard);
	if (val == 0x05 || val == 0x0a || val == 0x0d)
	{
		return val > 0x0a ? 0x0a : val;
	}
	return 0;
}
//获取一轮得分 返回得分牌明细和得分
int CGameLogic::GetScore(const _uint8 cbCards[], _uint8 nCount, _uint8 cbScoreCards[], _uint8 &nScoreCnt)
{
	if (NULL == cbCards || NULL == cbScoreCards)
	{
		return 0;
	}
	int iAllScore = 0, iTempScore = 0;
	//nScoreCnt = 0;
	for (int i = 0; i < nCount; i++)
	{
		iTempScore = GetScore(cbCards[i]);
		if (iTempScore != 0)
		{
			iAllScore += iTempScore;
			cbScoreCards[nScoreCnt++] = cbCards[i];
		}
	}
	return iAllScore;
}
//获取底牌得分
int CGameLogic::GetBackScore(const _uint8 cbBackCards[], _uint8 nBackCnt)
{
	if (NULL == cbBackCards)
	{
		return 0;
	}
	int iAllScore = 0, iTempScore = 0;
	for (int i = 0; i < nBackCnt; i++)
	{
		iTempScore = GetScore(cbBackCards[i]);
		if (iTempScore != 0)
		{
			iAllScore += iTempScore;
		}
	}
	return iAllScore;
}

//扑克转换
_uint8 CGameLogic::SwitchCardToUser(_uint8 cbCardData[], _uint8 bCardCount, _uint8 cbCardIndex[MAX_INDEX])
{
	//设置变量
	memset(cbCardIndex, 0, sizeof(_uint8) * MAX_INDEX);
	//转换扑克
	for (_uint8 i = 0; i < bCardCount; i++)
	{
		//assert(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}
	return bCardCount;
}

//扑克转换
_uint8 CGameLogic::SwitchToCardIndex(_uint8 cbCardData)
{
	//assert(IsValidCard(cbCardData));
	return ((cbCardData & MASK_COLOR) >> 4) * 13 + (cbCardData & MASK_VALUE) - 1;
}

//扑克转换
_uint8 CGameLogic::SwitchToCardData(_uint8 cbCardIndex)
{
	//assert(cbCardIndex<MAX_INDEX);
	return ((cbCardIndex / 13) << 4) | (cbCardIndex % 13 + 1);
}

//有效判断
bool CGameLogic::IsValidCard(_uint8 cbCardData)
{
	_uint8 cbValue = (cbCardData & MASK_VALUE);
	_uint8 cbColor = (cbCardData & MASK_COLOR) >> 4;
	return ((cbValue >= 1) && (cbValue <= 0x0D) && (cbColor <= 4));
}

bool CGameLogic::IsTrackerRule(_uint8 nCardFirst, _uint8 nCardNext)
{
	//return (1 == (GetWeight(nCardFirst) - GetWeight(nCardNext)));
	return (1 == (GetSortWeight(nCardFirst, m_nNTCCard, m_bNTC, 0, m_bNtcNum2, m_bDel34) - GetSortWeight(nCardNext, m_nNTCCard, m_bNTC, 0, m_bNtcNum2, m_bDel34)));
}

void CGameLogic::SortCard(_uint8 cbCards[], _uint8 nCount)
{
	if (NULL == cbCards)
	{
		return;
	}
	tagCard cards[PLAYER_CARD_CNT + BACK_CARD_CNT];

	for (_uint8 i = 0; i < nCount; ++i)
	{
		cards[i].card = cbCards[i];
		cards[i].ntCard = m_nNTCCard;
		cards[i].bNTC = m_bNTC;
		cards[i].CardDel34 = m_bDel34;
		cards[i].NTCNum2 = m_bNtcNum2;
	}
	qsort(cards, nCount, sizeof(tagCard), CGameLogic::CompareCard); //优化空间，基本有序，可以用插入排序
	for (_uint8 i = 0; i < nCount; ++i)
	{
		cbCards[i] = cards[i].card;
	}
}

int CGameLogic::CompareCard(const void *nCard1, const void *nCard2)
{
	return GetSortWeight(*(tagCard *)nCard2) - GetSortWeight(*(tagCard *)nCard1);
}

_tint32 CGameLogic::GetWeight(const _uint8 card_)
{
	//return GetWeight(card_ , m_nNTColor);
	return GetSortWeight(card_, m_nNTCCard, m_bNTC, 0, m_bNtcNum2, m_bDel34);
}

_tint32 CGameLogic::GetSortWeight(const tagCard &card)
{
	return GetSortWeight(card.card, card.ntCard, card.bNTC, 1, card.NTCNum2, card.CardDel34);
}
/*牌对应的权重，一张牌一个权重，用来对牌进行排序，或者牌型判断
12 52  1  2  3  4  5  6  7  8  9 10 11 
25 52 14 15 16 17 18 19 20 21 22 23 24 
38 52 27 28 29 30 31 32 33 34 35 36 37 
51 53 40 41 42 43 44 45 46 47 48 49 50 
54 55 
*/
_tint32 CGameLogic::GetSortWeight(const _uint8 card_, const _uint8 ntCard_, const bool bNTC, const _uint8 sort, const bool isNt2, const bool bDel34)
{
	if (!IsValidCard(card_))
	{
		return -1;
	}
	_uint8 ntCardNum = GetCardNum(ntCard_);
	if (isNt2 && 2 == ntCardNum) //2是常主，级牌又是2
	{
		return 0;
	}
	_uint8 card = card_;
	_uint8 nCardNum = GetCardNum(card_);
	if (bDel34 && (3 == nCardNum || 4 == nCardNum || 3 == ntCardNum || 4 == ntCardNum))
	{
		return 0;
	}
	_uint8 nCardColor = GetCardColor(card_);
	_uint8 ntColor = GetCardColor(ntCard_);
	_uint8 ntStart = ((isNt2 ? (bDel34 ? 49 : 51) : (bDel34 ? 50 : 52)) - (1 == ntCardNum));
	_uint8 A_addWeight = isNt2 ? (bDel34 ? 9 : 11) : (bDel34 ? 10 : 12);

	_uint8 nWeight = 0;

	if (nCardColor != 4)
	{
		if ((bNTC) && 3 != ntColor) //如果有主，主花色是3那就不用动
		{
			//主花色和3花色交换权重
			if (ntColor == nCardColor)
			{
				card = 0x30 + nCardNum;
				nCardColor = 3;
			}
			else if (ntColor < nCardColor)
			{
				nCardColor -= 1;
				card = (nCardColor << 4) + nCardNum;
			}
		}

		if (isNt2 && nCardNum == 2)
		{
			if ((!bNTC) || 3 != nCardColor)
			{ //副 级牌
				nWeight = ntStart + sort * (nCardColor);
			}
			else
			{ //主 级牌
				nWeight = ntStart + 1 + sort * (nCardColor);
			}
		}
		else if (nCardNum == ntCardNum)
		{
			if ((!bNTC) || 3 != nCardColor)
			{ //副 级牌
				nWeight = ntStart + (isNt2 ? (bNTC ? 2 : 1) : 0) + sort * (nCardColor + 4);
			}
			else
			{ //主 级牌
				nWeight = ntStart + (isNt2 ? (bNTC ? 2 : 1) : 0) + 1 + sort * (nCardColor + 4);
			}
		}
		else if (nCardNum == 1)
		{
			nWeight = SwitchToCardIndex(card) + A_addWeight;
		}
		else if (isNt2)
		{
			if (bDel34)
			{
				if (2 < nCardNum && nCardNum < ntCardNum)
				{
					nWeight = SwitchToCardIndex(card - 3);
				}
				else if (nCardNum > ntCardNum)
				{
					nWeight = SwitchToCardIndex(card - 4);
				}
			}
			else
			{
				if (2 < nCardNum && nCardNum < ntCardNum)
				{
					nWeight = SwitchToCardIndex(card - 1);
				}
				else if (nCardNum > ntCardNum)
				{
					nWeight = SwitchToCardIndex(card - 2);
				}
			}
		}
		else
		{
			if (bDel34)
			{
				if (1 == ntCardNum || 2 == ntCardNum)
				{
					if (nCardNum > 2)
					{
						nWeight = SwitchToCardIndex(card - 3);
					}
					else
					{
						nWeight = SwitchToCardIndex(card - 1);
					}
				}
				else
				{
					if (nCardNum > ntCardNum)
					{
						nWeight = SwitchToCardIndex(card - 3);
					}
					else if (nCardNum > 2)
					{
						nWeight = SwitchToCardIndex(card - 2);
					}
					else
					{
						nWeight = SwitchToCardIndex(card);
					}
				}
			}
			else
			{
				if (nCardNum > ntCardNum)
				{
					nWeight = SwitchToCardIndex(card - 1);
				}
				else
				{
					nWeight = SwitchToCardIndex(card);
				}
			}
		}
	}
	else
	{
		nWeight = ntStart + (isNt2 ? (bNTC ? 3 : 1) : (bNTC ? 1 : 0)) + nCardNum + sort * (nCardColor + 8);
	}
	return nWeight;
}

_tint32 CGameLogic::GetSortWeightOld(bool CardDel34, bool NTCNum2, const _uint8 card_, const _uint8 ntCard_, const bool bNTC, const _uint8 sort)
{
	if (!IsValidCard(card_))
	{
		return 0;
	}
	_uint8 card = card_;
	_uint8 nCardNum = GetCardNum(card_);
	_uint8 nCardColor = GetCardColor(card_);
	_uint8 ntColor = GetCardColor(ntCard_);
	_uint8 ntCardNum = GetCardNum(ntCard_);
	_uint8 nWeight = 0;

	_uint8 nMaxWeight = 52;
	_uint8 nFeiJi = 0;
	_uint8 paixu = 0;
	_uint8 Del34 = 0;
	if (NTCNum2)
	{
		nMaxWeight = 53;
		nFeiJi = 1;
	}
	if (1 == sort)
	{
		paixu = 2;
	}
	if (CardDel34)
	{
		Del34 = 2;
	}

	if (nCardColor != 4)
	{
		if ((bNTC) && 3 != ntColor) //如果有主，主花色是3那就不用动
		{
			//主花色和3花色交换权重
			if (ntColor == nCardColor)
			{
				card = 0x30 + nCardNum;
				nCardColor = 3;
			}
			else if (ntColor < nCardColor)
			{
				nCardColor -= 1;
				card = (nCardColor << 4) + nCardNum;
			}
		}
		if (nCardNum == 1 && (1 != ntCardNum))
		{ //A
			nWeight = SwitchToCardIndex(card) + 12 - nFeiJi;
		}
		else if (NTCNum2 && nCardNum == 2)
		{
			if ((!bNTC) || 3 != nCardColor)
			{ //副 级牌
				nWeight = nMaxWeight + (sort * nCardColor) - 2;
			}
			else
			{ //主 级牌
				nWeight = (nMaxWeight + 1) + (sort * 2) - 2;
			}
		}
		else if (nCardNum == ntCardNum)
		{
			if ((!bNTC) || 3 != nCardColor)
			{ //副 级牌
				nWeight = nMaxWeight + (sort * nCardColor) + paixu;
			}
			else
			{ //主 级牌
				nWeight = (nMaxWeight + 1) + (sort * 2) + paixu;
			}
		}
		else if (nCardNum > ntCardNum)
		{
			nWeight = SwitchToCardIndex(card - 1 - nFeiJi);
		}
		else
		{
			nWeight = SwitchToCardIndex(card - nFeiJi);
		}
	}
	else
	{
		nWeight = nMaxWeight + (bNTC ? 1 : 0) + nCardNum + (sort * 3) + paixu;
	}

	if (CardDel34 && (!NTCNum2))
	{
		if ((5 <= nCardNum) && (13 >= nCardNum) && (nCardNum != ntCardNum))
		{
			nWeight -= 2;
		}
		if (1 == nCardNum && 4 != nCardColor)
		{
			nWeight -= 2;
		}
	}

	return nWeight;
}

//获取牌型
_uint8 CGameLogic::GetCardType(const _uint8 cbCards[], _uint8 nCount)
{
	if (nCount == 0)
	{
		return TYPE_ERROR;
	}
	if (IsSingle(cbCards, nCount))
	{
		return TYPE_SINGLE;
	}
	if (IsDouble(cbCards, nCount))
	{
		return TYPE_DOUBLE;
	}
	if (!GetCardListColor(cbCards, nCount))
	{ //不是同一花色，没必要继续判断
		return TYPE_ERROR;
	}
	if (IsTracker(cbCards, nCount))
	{
		return TYPE_TRACKER;
	}

	return TYPE_FLING; //IsFling(cbCards, nCount);
}

//判断一列牌是否是同一个花色
bool CGameLogic::GetCardListColor(const _uint8 cbCards[], _uint8 nCount)
{
	if (nCount == 0)
	{
		return false;
	}
	for (int i = 1; i < nCount; ++i)
	{
		if (!SameClr(cbCards[i], cbCards[0]))
		{
			return false;
		}
	}
	return true;
}

//洗牌
void CGameLogic::RandCard(_uint8 cbCards[])
{
	if (NULL == cbCards)
	{
		return;
	}
	_uint8 cbCardsTemp[TOTAL_CARD_CNT];
	_uint8 totalCardCnt = TOTAL_CARD_CNT;
	srand(time(NULL));
	if (m_bDel34)
	{
		_uint8 cardCnt = 0;
		for (int i = 0; i < 108; ++i)
		{
			if (3 != GetCardNum(g_cbCards[i % 54]) && 4 != GetCardNum(g_cbCards[i % 54]))
			{
				cbCardsTemp[cardCnt++] = g_cbCards[i % 54];
			}
			//memcpy(&cbCardsTemp[54 * i], g_cbCards, sizeof(g_cbCards));
		}
		totalCardCnt = TOTAL_CARD_CNT34;
	}
	else
	{
		for (int i = 0; i < TOTAL_CARD_CNT / 54; ++i)
		{
			memcpy(&cbCardsTemp[54 * i], g_cbCards, sizeof(g_cbCards));
		}
	}

	_uint8 cSend = 0, cStation = 0;
	do
	{
		cStation = rand() % (totalCardCnt - cSend);
		cbCards[cSend++] = cbCardsTemp[cStation];
		cbCardsTemp[cStation] = cbCardsTemp[totalCardCnt - cSend];
	} while (cSend < totalCardCnt);
}

//从手牌中删除牌
_uint8 CGameLogic::RemoveCard(_uint8 cbRemoveCards[], _uint8 cRemoveCnt, _uint8 cbHandCards[], _uint8 &cHandCnt)
{
	if (NULL == cbRemoveCards || NULL == cbHandCards)
	{
		return 0;
	}
	if (cRemoveCnt > cHandCnt)
	{
		return 0;
	}
	_uint8 cDelCnt = 0;
	for (int i = 0; i < cRemoveCnt; ++i)
	{
		for (int j = 0; j < cHandCnt; ++j)
		{
			if (cbRemoveCards[i] == cbHandCards[j])
			{
				cDelCnt++;
				cbHandCards[j] = 0;
				break;
			}
		}
	}
	//RemoveNoCard(cbHandCards, cHandCnt);
	if (cDelCnt != cRemoveCnt)
	{
		return 0;
	}
	SortCard(cbHandCards, cHandCnt);
	cHandCnt -= cRemoveCnt;
	return cDelCnt;
}

//清理数量为0的牌
_uint8 CGameLogic::RemoveNoCard(_uint8 cbHandCards[], _uint8 cHandCnt)
{
	_uint8 cDelCnt = 0;
	for (int i = 0; i < cHandCnt; ++i)
	{
		if (cbHandCards[i] != 0)
		{
			cbHandCards[i - cDelCnt] = cbHandCards[i];
		}
		else
		{
			cDelCnt++;
		}
	}
	return cDelCnt;
}


_uint8 CGameLogic::GetCardColor(_uint8 nCard)
{
	return (nCard & MASK_COLOR) >> 4;
}

//下面两个函数要求cbCards按权重从大到小排序，并且花色一致
bool CGameLogic::IsTracker(const _uint8 cbCards[], _uint8 nCount)
{
	if (nCount < 4 || nCount % 2 != 0)
	{
		return false;
	}

	if (!IsDouble(cbCards, 2))
	{
		return false;
	}
	_uint8 nCard = cbCards[0];
	for (_uint8 i = 2; i < nCount; i += 2)
	{
		if (!IsDouble(&cbCards[i], 2) || !IsTrackerRule(nCard, cbCards[i]))
		{
			return false;
		}
		nCard = cbCards[i];
	}
	return true;
}

//是否是甩牌
_uint8 CGameLogic::IsFling(const _uint8 cbCards[], _uint8 nCount)
{
	if ((nCount >= 2))
	{
		_uint8 i = 0;
		for (; i < nCount - 1; ++i)
		{
			if (cbCards[i] == cbCards[i + 1]) //有对子
			{
				break;
			}
		}
		if ((nCount - 1) <= i)
		{ //全是单张甩牌
			return TYPE_FLING;
		}
		for (_uint8 i = 0; i < nCount; i += 2)
		{
			if (!IsDouble(&cbCards[i], 2))
			{
				//不全是单张，也不全是对子
				return TYPE_ERROR;
			}
		}
		return TYPE_FLING_DB;
	}
	return TYPE_ERROR;
}

//都是甩牌，比较大小
bool CGameLogic::CompareFlingCard(const _uint8 cbOutCards_Lps[], const _uint8 cbOutCards_Rps[], _uint8 nOutCnt)
{
	_uint8 Lps_db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 Lps_dbCnt = 0;
	GetAllDBByClr(cbOutCards_Lps, nOutCnt, NULL, NULL, Lps_db, &Lps_dbCnt, NULL, NULL, cbOutCards_Lps[0]);

	_uint8 Rps_db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 Rps_dbCnt = 0;
	GetAllDBByClr(cbOutCards_Rps, nOutCnt, NULL, NULL, Rps_db, &Rps_dbCnt, NULL, NULL, cbOutCards_Rps[0]);

	if (Lps_dbCnt > 0 && Rps_dbCnt < Lps_dbCnt)
	{
		//后出的一家，对子数量没有前面一家多，直接为跟牌
		return true;
	}

	_uint8 Lps_tkCnt = 0;
	_uint8 Lps_start[COLOR_CARD_CNT / 2];  //起始位置
	_uint8 Lps_length[COLOR_CARD_CNT / 2]; //拖拉机长度
	GetTracker(Lps_db, Lps_dbCnt, Lps_start, Lps_length, Lps_tkCnt);

	_uint8 Rps_tkCnt = 0;
	_uint8 Rps_start[COLOR_CARD_CNT / 2];  //起始位置
	_uint8 Rps_length[COLOR_CARD_CNT / 2]; //拖拉机长度
	GetTracker(Rps_db, Rps_dbCnt, Rps_start, Rps_length, Rps_tkCnt);

	//先比较拖拉机，再比较对子，最后单牌

	if (0 < Lps_tkCnt)
	{
		//拖拉机的总长度要一致
		_uint8 Lps_TK_len = 0;
		_uint8 Rps_TK_len = 0;
		for (int i = 0; i < Lps_dbCnt; ++i)
		{
			Lps_TK_len += Lps_length[i];
		}
		for (int i = 0; i < Rps_dbCnt; ++i)
		{
			Rps_TK_len += Rps_length[i];
		}

		if (Lps_TK_len > 0)
		{
			//有拖拉机，判断拖拉机大小
			if (Rps_TK_len < Lps_TK_len)
			{
				return true;
			}
			else
			{
				//比较两家最长的拖拉机
				_uint8 Lps_MaxLenPos = Lps_start[0];
				_uint8 Rps_MaxLenPos = Rps_start[0];
				_uint8 Lps_MaxLen = Lps_length[0];
				_uint8 Rps_MaxLen = Rps_length[0];
				for (int i = 1; i < Lps_dbCnt; ++i)
				{
					if (Lps_MaxLen < Lps_length[i])
					{
						Lps_MaxLen = Lps_length[i];
						Lps_MaxLenPos = Lps_start[i];
					}
				}
				for (int i = 1; i < Rps_dbCnt; ++i)
				{
					if (Rps_MaxLen < Rps_length[i])
					{
						Rps_MaxLen = Rps_length[i];
						Rps_MaxLenPos = Rps_start[i];
					}
				}

				if (Rps_MaxLen < Lps_MaxLen)
				{
					//如果跟牌的人的最长拖拉机没有上家的大，那么算垫牌
					return true;
				}
				else
				{
					return Compare(Lps_db[Lps_MaxLenPos], Rps_db[Rps_MaxLenPos]);
				}
			}
		}
	}

	//没有拖拉机，判断对子,前面已经判断对子数量，跟牌玩家不会比先出的人少
	if (Lps_dbCnt > 0)
	{
		//比较最大的对子
		return Compare(Lps_db[0], Rps_db[0]);
	}

	//比较单牌
	return Compare(cbOutCards_Lps[0], cbOutCards_Rps[0]);
}

//是否可以甩牌 必须是同花色  甩牌失败 强制出最小的牌
//牌均已排好序，才能调用这个函数
bool CGameLogic::CanFlingCard(const _uint8 cbOutCards[], _uint8 nOutCnt, _tint32 nOutChair,
							  const _uint8 cbOtherCards[][PLAYER_CARD_CNT], _uint8 cbGamePlayer, const _uint8 nCardCnt[],
							  _uint8 cbResultCards[], _uint8 &nResultCnt, _uint8 &type)
{
	if (NULL == cbOutCards || NULL == cbOtherCards || NULL == nCardCnt || NULL == cbResultCards)
	{
		return false;
	}
	type = GetCardType(cbOutCards, nOutCnt);
	if (TYPE_ERROR == type)
	{
		return false;
	}
	nResultCnt = 0;
	if (TYPE_FLING == type)
	{
		_uint8 sg[COLOR_CARD_CNT]; //COLOR_CARD_CNTZ张同花色单牌
		_uint8 sgCnt = 0;
		_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
		_uint8 dbCnt = 0;

		GetAllDBByClr(cbOutCards, nOutCnt, sg, &sgCnt, db, &dbCnt, NULL, NULL, cbOutCards[0]);

		for (int i = 0; i < cbGamePlayer && i < GAME_PLAYER; ++i)
		{
			//不和自己比
			if (nOutChair == i)
			{
				continue;
			}

			if (0 < sgCnt)
			{
				//甩牌有单张
				if (!CompareFlingSGCard(sg[sgCnt - 1], nOutCnt, cbOtherCards[i], nCardCnt[i]))
				{
					nResultCnt = 1;
					cbResultCards[0] = sg[sgCnt - 1];
					return true;
				}
			}

			if (0 < dbCnt)
			{
				//对子和拖拉机
				if (CompareFlingDBCard(db, dbCnt, nOutCnt, cbOtherCards[i], nCardCnt[i], cbResultCards, nResultCnt))
				{
					return true;
				}
			}
		}
	}
	return true;
}

bool CGameLogic::CompareFlingSGCard(_uint8 sgMinCard, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt)
{
	//_uint8 cnt = GetSameColorNum(otherCards, otherCardsCnt, sgMinCard);
	for (_uint8 i = 0; i < otherCardsCnt; ++i)
	{
		//((((false) && true) || false))
		//bool bntc = NTC(otherCards[i]);
		bool bsame = SameClr(sgMinCard, otherCards[i]);
		if (bsame && !Compare(sgMinCard, otherCards[i]))
		{
			return false;
		}
	}
	return true;
}

bool CGameLogic::CompareFlingDBCard(const _uint8 db[], _uint8 dbCnt, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt, _uint8 cbResultCards[], _uint8 &nResultCnt)
{
	_uint8 tmpdb[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 tmpdbCnt = 0;

	GetAllDBByClr(otherCards, otherCardsCnt, NULL, NULL, tmpdb, &tmpdbCnt, NULL, NULL, db[0]);

	//对子甩牌
	_uint8 tkCnt = 0;
	_uint8 start[COLOR_CARD_CNT / 2];  //起始位置
	_uint8 length[COLOR_CARD_CNT / 2]; //拖拉机长度

	GetTracker(db, dbCnt, start, length, tkCnt);

	//找到最小的对子牌
	_uint8 minDBCard = INVALID_CARD;
	for (int i = dbCnt - 1; i >= 0; --i)
	{
		bool flag = false;
		for (_uint8 j = 0; j < tkCnt; ++j)
		{
			if (start[j] <= i && i < (start[j] + length[j]))
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			//从后往前找，找到第一张就是最小的
			minDBCard = db[i];
			break;
		}
	}

	if (minDBCard != INVALID_CARD)
	{
		//只要有一个对子,比甩的对子大，那就甩牌失败
		for (_uint8 i = 0; i < tmpdbCnt; ++i)
		{
			if (!Compare(minDBCard, tmpdb[i]))
			{
				nResultCnt = 2;
				cbResultCards[0] = minDBCard;
				cbResultCards[1] = minDBCard;
				return true;
			}
		}
	}

	//判断拖拉机是不是大的
	if (0 < tkCnt)
	{
		///考虑同花色牌数量不够，用主拖来机毙的情况，前面对子或单牌已经能判断了。不会有这种情况
		_uint8 tmpTkCnt = 0;
		_uint8 tmpStart[COLOR_CARD_CNT / 2];  //起始位置
		_uint8 tmpLength[COLOR_CARD_CNT / 2]; //拖拉机长度

		GetTracker(tmpdb, tmpdbCnt, tmpStart, tmpLength, tmpTkCnt);

		for (int i = tkCnt - 1; i >= 0; --i)
		{
			for (_uint8 j = 0; j < tmpTkCnt; ++j)
			{
				if (length[i] <= tmpLength[j])
				{
					if (!Compare(db[start[i]], tmpdb[tmpStart[j]]))
					{
						//不能甩牌,start[i],length[i]
						//nResultCnt = length[i];
						for (_uint8 k = start[i]; k < (start[i] + length[i]); ++k)
						{
							cbResultCards[nResultCnt++] = db[k];
							cbResultCards[nResultCnt++] = db[k];
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool CGameLogic::CompareFlingSGCardOld(_uint8 sgMinCard, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt)
{
	_uint8 cnt = GetSameColorNum(otherCards, otherCardsCnt, sgMinCard);
	for (_uint8 i = 0; i < otherCardsCnt; ++i)
	{
		//((((false) && true) || false))
		bool bntc = NTC(otherCards[i]);
		bool bsame = SameClr(sgMinCard, otherCards[i]);
		if ((((cnt < nOutCnt) && bntc) || bsame) && !Compare(sgMinCard, otherCards[i]))
		{
			return false;
		}
	}
	return true;
}

bool CGameLogic::CompareFlingDBCardOld(const _uint8 db[], _uint8 dbCnt, _uint8 nOutCnt, const _uint8 otherCards[], _uint8 otherCardsCnt, _uint8 cbResultCards[], _uint8 &nResultCnt)
{
	_uint8 tmpdb[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 tmpdbCnt = 0;
	_uint8 tmpmdb[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 tmpmdbCnt = 0;

	GetAllDBByClr(otherCards, otherCardsCnt, NULL, NULL, tmpdb, &tmpdbCnt, tmpmdb, &tmpmdbCnt, db[0]);

	//对子甩牌
	_uint8 tkCnt = 0;
	_uint8 start[COLOR_CARD_CNT / 2];  //起始位置
	_uint8 length[COLOR_CARD_CNT / 2]; //拖拉机长度

	GetTracker(db, dbCnt, start, length, tkCnt);

	//找到最小的对子牌
	_uint8 minDBCard = INVALID_CARD;
	for (int i = dbCnt - 1; i >= 0; --i)
	{
		bool flag = false;
		for (_uint8 j = 0; j < tkCnt; ++j)
		{
			if (start[j] <= i && i < (start[j] + length[j]))
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			//从后往前找，找到第一张就是最小的
			minDBCard = db[i];
			break;
		}
	}
	_uint8 cnt = GetSameColorNum(otherCards, otherCardsCnt, db[0]);
	if (minDBCard != INVALID_CARD)
	{
		//判断对子是不是大的
		//手上这个花色全是对子,并且对子数量不够，只要有主对，那就甩牌失败
		if (cnt <= (nOutCnt - 2) && (!NTC(db[0])) && (0 < tmpmdbCnt))
		{
			nResultCnt = 2;
			cbResultCards[0] = minDBCard;
			cbResultCards[1] = minDBCard;
			return true;
		}
		else
		{
			//只要有一个对子,比甩的对子大，那就甩牌失败
			for (_uint8 i = 0; i < tmpdbCnt; ++i)
			{
				if (!Compare(minDBCard, tmpdb[i]))
				{
					nResultCnt = 2;
					cbResultCards[0] = minDBCard;
					cbResultCards[1] = minDBCard;
					return true;
				}
			}
		}
	}

	//判断拖拉机是不是大的
	if (0 < tkCnt)
	{
		///考虑同花色牌数量不够，用主拖来机毙的情况，前面对子或单牌已经能判断了。不会有这种情况

		//如果牌的数量不够，就不用判断了
		if (cnt >= nOutCnt)
		{
			_uint8 tmpTkCnt = 0;
			_uint8 tmpStart[COLOR_CARD_CNT / 2];  //起始位置
			_uint8 tmpLength[COLOR_CARD_CNT / 2]; //拖拉机长度

			GetTracker(tmpdb, tmpdbCnt, tmpStart, tmpLength, tmpTkCnt);

			for (int i = tkCnt - 1; i >= 0; --i)
			{
				for (_uint8 j = 0; j < tmpTkCnt; ++j)
				{
					if (length[i] <= tmpLength[j])
					{
						if (!Compare(db[start[i]], tmpdb[tmpStart[j]]))
						{
							//不能甩牌,start[i],length[i]
							//nResultCnt = length[i];
							for (_uint8 k = start[i]; k < (start[i] + length[i]); ++k)
							{
								cbResultCards[nResultCnt++] = db[k];
								cbResultCards[nResultCnt++] = db[k];
							}
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CGameLogic::CompareOutCard(const _uint8 cbOutCards_Lps[], const _uint8 cbROutCards_Rps[], _uint8 nOutCnt)
{
	if (NULL == cbOutCards_Lps || NULL == cbROutCards_Rps || nOutCnt > PLAYER_CARD_CNT)
	{
		return false;
	}
	_uint8 lpstType = GetCardType(cbOutCards_Lps, nOutCnt);
	bool res = true;
	switch (lpstType)
	{
	case TYPE_SINGLE:
	case TYPE_DOUBLE:
	case TYPE_TRACKER:
	{
		if (lpstType == GetCardType(cbROutCards_Rps, nOutCnt) && !Compare(cbOutCards_Lps[0], cbROutCards_Rps[0]))
		{
			res = false;
		}
	}
	break;
	case TYPE_FLING: //如果能甩牌，就已经在之前判断甩出来的牌最大
	{
		if (lpstType == GetCardType(cbROutCards_Rps, nOutCnt))
		{
			res = CompareFlingCard(cbOutCards_Lps, cbROutCards_Rps, nOutCnt);
		}
	}
	break;
	default: //正常情况下不会出现
		break;
	}
	return res;
}

//获取同花色的牌还有多少张
_uint8 CGameLogic::GetSameColorNum(const _uint8 cbCards[], _uint8 nCount, _uint8 card)
{
	_uint8 cnt = 0;
	//如果是主花色，还要算大小王、2
	for (_uint8 i = 0; i < nCount; ++i)
	{
		if (SameClr(cbCards[i], card))
		{
			cnt++;
		}
	}
	return cnt;
}
/*
* 获取同一花色所有对子
* const _uint8 cards[],	组合牌，排好序的
* _uint8 cardsCnt,		组合牌数量
* _uint8 sg[], 			存放单牌
* _uint8* sgCnt, 		单牌数量
* _uint8 db[],			对子，只存放单张
* _uint8* dbCnt, 		对子数量
* _uint8 mdb[],			主对，只存放单张
* _uint8* mdbCnt, 		主对数量
* _uint8 card			当前牌，获取跟此牌同一花色的单牌，对子，如果这张是主牌，那mdb为空
*/
_uint8 CGameLogic::GetAllDBByClr(const _uint8 cards[], _uint8 cardsCnt, _uint8 sg[], _uint8 *sgCnt, _uint8 db[], _uint8 *dbCnt, _uint8 mdb[], _uint8 *mdbCnt, _uint8 card)
{
	(NULL != dbCnt) && (*dbCnt = 0);   //对子数量
	(NULL != mdbCnt) && (*mdbCnt = 0); //主对数量
	(NULL != sgCnt) && (*sgCnt = 0);   //单牌数量
	for (_uint8 i = 0; i < cardsCnt; ++i)
	{
		if (SameClr(cards[i], card))
		{
			//如果有对子
			if ((i < (cardsCnt - 1)) && IsDouble(&cards[i], 2))
			{
				if (NULL != db && NULL != dbCnt)
				{
					db[*dbCnt] = cards[i];
					*dbCnt += 1;
				}
				++i;
			}
			else
			{
				if (NULL != sg && NULL != sgCnt)
				{
					sg[*sgCnt] = cards[i];
					*sgCnt += 1;
				}
			}
		}
		else if (NTC(cards[i]) && !NTC(card) && (i < (cardsCnt - 1)) && IsDouble(&cards[i], 2))
		{
			if (NULL != mdb && NULL != mdbCnt)
			{
				mdb[*mdbCnt] = cards[i];
				*mdbCnt += 1;
			}
		}
	}
	return (((NULL != dbCnt) ? (*dbCnt) : 0) + ((NULL != mdbCnt) ? (*mdbCnt) : 0));
}

_uint8 CGameLogic::GetCardCnt(_uint8 Card, const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt)
{
	_uint8 cnt = 0;
	for (_uint8 i = 0; i < nCardCnt; ++i)
	{
		if (Card == cbCards[i])
		{
			cnt++;
		}
	}
	return cnt;
}

/*
const _uint8 db[],	对子，只存放单张
_uint8 dbCnt, 		对子数量
_uint8 start[], 	拖拉机起始位置
_uint8 length[], 	拖拉机长度
_uint8 &cnt			拖来机数量
*/
bool CGameLogic::GetTracker(const _uint8 db[], _uint8 dbCnt, _uint8 start[], _uint8 length[], _uint8 &cnt)
{
	cnt = 0;
	//找到拖拉机
	start[cnt] = 255;
	length[cnt] = 1;
	bool flag = false;
	for (_uint8 i = 0; i < dbCnt - 1; ++i)
	{
		if (IsTrackerRule(db[i], db[i + 1]))
		{
			if (255 == start[cnt])
			{
				flag = true;
				start[cnt] = i;
			}
			++length[cnt];
		}
		else if (255 != start[cnt])
		{
			flag = false;
			++cnt;
			start[cnt] = 255;
			length[cnt] = 1;
		}
	}
	if (flag)
	{
		++cnt;
	}
	return true;
}

//*******************************************************************************

//要求牌已按从大到小排序
_uint8 CGameLogic::GetOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt,
							   _uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 type = GetCardType(outCards, outCardsCnt);
	mustOutCnt = 0;
	canOutCnt = 0;
	canOutClrCnt = 0;
	switch (type)
	{
	case TYPE_SINGLE:
	{
		GetSGOutCards(outCards[0], cards, cardsCnt, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);
	}
	break;
	case TYPE_DOUBLE:
	{
		GetDBOutCards(outCards[0], cards, cardsCnt, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);
	}
	break;
	case TYPE_TRACKER:
	{
		GetTKOutCards(outCards, outCardsCnt, cards, cardsCnt, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);
	}
	break;
	case TYPE_FLING:
	{
		GetFLOutCards(outCards, outCardsCnt, cards, cardsCnt, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);
	}
	break;
	case TYPE_ERROR:
	default:
		break;
	}
	return type;
}

/*
* 单牌，大小王和2默认可以出
*/
bool CGameLogic::GetSGOutCards(_uint8 outCard, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
							   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 cnt = GetSameColorNum(cards, cardsCnt, outCard);
	if (cnt > 0)
	{
		if (NTC(outCard))
		{
			m_bNTC && (canOutClr[canOutClrCnt++] = m_nNTColor);
			for (_uint8 i = 0; i < cardsCnt; ++i)
			{
				if (SpNTC(cards[i]))
				{
					canOutCard[canOutCnt++] = cards[i];
				}
			}
		}
		else
		{
			canOutClr[canOutClrCnt++] = GetCardColor(outCard);
		}
	}
	else
	{
		for (_uint8 i = 0; i <= 4; ++i)
		{
			{
				canOutClr[canOutClrCnt++] = i;
			}
		}
	}
	return true;
}
//对子
bool CGameLogic::GetDBOutCards(_uint8 outCard, const _uint8 cards[], _uint8 cardsCnt, _uint8 mustOutCard[],
							   _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 cnt = GetSameColorNum(cards, cardsCnt, outCard);
	if (cnt >= 2)
	{
		_uint8 sg[COLOR_CARD_CNT]; //COLOR_CARD_CNTZ张同花色单牌
		_uint8 sgCnt = 0;
		_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
		_uint8 dbCnt = 0;
		GetAllDBByClr(cards, cardsCnt, sg, &sgCnt, db, &dbCnt, NULL, NULL, outCard);
		for (_uint8 i = 0; i < dbCnt; ++i)
		{
			//有对子可以出，那就只能出对子
			canOutCard[canOutCnt++] = db[i];
		}
		if (0 == canOutCnt)
		{
			//没有对子可以出，本花色的牌可以随便出
			if (NTC(outCard))
			{
				m_bNTC && (canOutClr[canOutClrCnt++] = m_nNTColor);
				for (_uint8 i = 0; i < sgCnt; ++i)
				{
					if (SpNTC(sg[i]))
					{
						canOutCard[canOutCnt++] = sg[i];
					}
				}
			}
			else
			{
				canOutClr[canOutClrCnt++] = GetCardColor(outCard);
			}
		}
	}
	else
	{
		//本花色的牌，必须出，其他花色可以来填
		for (_uint8 i = 0; i < cardsCnt; ++i)
		{
			if (SameClr(cards[i], outCard))
			{
				mustOutCard[mustOutCnt++] = cards[i];
				if (IsDouble(&cards[i], 2))
				{
					++i;
				}
			}
		}
		for (_uint8 i = 0; i <= 4; ++i)
		{
			{
				canOutClr[canOutClrCnt++] = i;
			}
		}
	}
	return true;
}
//拖拉机
bool CGameLogic::GetTKOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt,
							   _uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 cnt = GetSameColorNum(cards, cardsCnt, outCards[0]);

	if (cnt >= outCardsCnt)
	{
		_uint8 sg[COLOR_CARD_CNT]; //COLOR_CARD_CNTZ张同花色单牌
		_uint8 sgCnt = 0;
		_uint8 tkCnt = outCardsCnt / 2;
		_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
		_uint8 dbCnt = 0;

		GetAllDBByClr(cards, cardsCnt, sg, &sgCnt, db, &dbCnt, NULL, NULL, outCards[0]);

		if (dbCnt < tkCnt)
		{
			//不可能有要的起的拖拉机,所有的对子必须出
			for (_uint8 j = 0; j < dbCnt; ++j)
			{
				mustOutCard[mustOutCnt++] = db[j];
			}
			//剩下的可以拿同花色任意牌来填
			if (NTC(outCards[0]))
			{
				m_bNTC && (canOutClr[canOutClrCnt++] = m_nNTColor);
				for (_uint8 i = 0; i < sgCnt; ++i)
				{
					if (SpNTC(sg[i]))
					{
						canOutCard[canOutCnt++] = sg[i];
					}
				}
			}
			else
			{
				canOutClr[canOutClrCnt++] = GetCardColor(outCards[0]);
			}
		}
		else
		{
			//对子数量够，只要出足够的对子就行
			for (_uint8 j = 0; j < dbCnt; ++j)
			{
				canOutCard[canOutCnt++] = db[j];
			}
		}
	}
	else
	{
		//同花色的牌不够，同花色的牌都要出，其他花色随便凑
		for (_uint8 i = 0; i < cardsCnt; ++i)
		{
			if (SameClr(cards[i], outCards[0]))
			{
				mustOutCard[mustOutCnt++] = cards[i];
				if (IsDouble(&cards[i], 2))
				{
					++i;
				}
			}
		}
		for (_uint8 i = 0; i <= 4; ++i)
		{
			{
				canOutClr[canOutClrCnt++] = i;
			}
		}
	}

	return true;
}
bool CGameLogic::GetFLOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt,
							   _uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 sg[COLOR_CARD_CNT]; //COLOR_CARD_CNTZ张同花色单牌
	_uint8 sgCnt = 0;
	_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 dbCnt = 0;

	if (outCardsCnt <= GetSameColorNum(cards, cardsCnt, outCards[0]))
	{
		GetAllDBByClr(outCards, outCardsCnt, sg, &sgCnt, db, &dbCnt, NULL, NULL, outCards[0]);

		if (0 < dbCnt)
		{
			GetFDOutCards(db, dbCnt, cards, cardsCnt, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);
		}

		//剩下的可以拿同花色任意牌来填
		if (NTC(outCards[0]))
		{
			m_bNTC && (canOutClr[canOutClrCnt++] = m_nNTColor);
			for (_uint8 i = 0; i < sgCnt; ++i)
			{
				if (SpNTC(sg[i]))
				{
					canOutCard[canOutCnt++] = sg[i];
				}
			}
		}
		else
		{
			canOutClr[canOutClrCnt++] = GetCardColor(outCards[0]);
		}
	}
	else
	{
		//同花色的牌不够，同花色的牌都要出，其他花色随便凑
		for (_uint8 i = 0; i < cardsCnt; ++i)
		{
			if (SameClr(cards[i], outCards[0]))
			{
				mustOutCard[mustOutCnt++] = cards[i];
				if (IsDouble(&cards[i], 2))
				{
					++i;
				}
			}
		}
		for (_uint8 i = 0; i <= 4; ++i)
		{
			{
				canOutClr[canOutClrCnt++] = i;
			}
		}
	}

	return true;
}
//对子甩牌
bool CGameLogic::GetFDOutCards(const _uint8 outDB[], _uint8 outDBCnt, const _uint8 cards[], _uint8 cardsCnt,
							   _uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
	_uint8 dbCnt = 0;

	GetAllDBByClr(cards, cardsCnt, NULL, NULL, db, &dbCnt, NULL, NULL, outDB[0]);

	if (dbCnt < outDBCnt)
	{
		//不可能有要的起的拖拉机,所有的对子必须出
		for (_uint8 j = 0; j < dbCnt; ++j)
		{
			mustOutCard[mustOutCnt++] = db[j];
		}
	}
	else
	{
		//对子数量够，只要出足够的对子就行
		for (_uint8 j = 0; j < dbCnt; ++j)
		{
			canOutCard[canOutCnt++] = db[j];
		}
	}

	return true;
}

bool CGameLogic::GetFSOutCards(const _uint8 outCards[], _uint8 outCardsCnt, const _uint8 cards[], _uint8 cardsCnt,
							   _uint8 mustOutCard[], _uint8 &mustOutCnt, _uint8 canOutCard[], _uint8 &canOutCnt, _uint8 canOutClr[], _uint8 &canOutClrCnt)
{
	_uint8 cnt = GetSameColorNum(cards, cardsCnt, outCards[0]);
	if (cnt >= outCardsCnt)
	{
		if (NTC(outCards[0]))
		{
			m_bNTC && (canOutClr[canOutClrCnt++] = m_nNTColor);
			for (_uint8 i = 0; i < cardsCnt; ++i)
			{
				if (SpNTC(cards[i]))
				{
					canOutCard[canOutCnt++] = cards[i];
				}
			}
		}
		else
		{
			canOutClr[canOutClrCnt++] = GetCardColor(outCards[0]);
		}
	}
	else
	{
		for (_uint8 i = 0; i < cardsCnt; ++i)
		{
			if (SameClr(cards[i], outCards[0]))
			{
				mustOutCard[mustOutCnt++] = cards[i];
				if (IsDouble(&cards[i], 2))
				{
					++i;
				}
			}
		}
		for (_uint8 i = 0; i <= 4; ++i)
		{
			{
				canOutClr[canOutClrCnt++] = i;
			}
		}
	}
	return true;
}

bool CGameLogic::CanOutCard(const _uint8 cbOutCards[], _uint8 nOutCnt, const _uint8 cbFirstCards[], _uint8 nFirstCardsCnt,
							const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt)
{
	if (nOutCnt != nFirstCardsCnt)
	{
		return false;
	}

	_uint8 UserOutCard[PLAYER_CARD_CNT]; //玩家出牌
	memcpy(UserOutCard, cbOutCards, sizeof(UserOutCard));

	_uint8 mustOutCard[PLAYER_CARD_CNT / 2];
	_uint8 mustOutCnt = 0;
	_uint8 canOutCard[PLAYER_CARD_CNT / 2];
	_uint8 canOutCnt = 0;
	_uint8 canOutClr[5];
	_uint8 canOutClrCnt = 0;

	//_uint8 type =
	GetOutCards(cbFirstCards, nFirstCardsCnt, cbCards, nCardCnt,
				mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);

	//简单判断，出的牌是不是符合规则

	if (!judgeMust(mustOutCard, mustOutCnt, UserOutCard, nOutCnt))
	{
		return false;
	}

	if (!judgeCan(canOutCard, canOutCnt, UserOutCard, nOutCnt, cbCards, nCardCnt))
	{
		return false;
	}

	if (!judgeClr(canOutClr, canOutClrCnt, UserOutCard, nOutCnt))
	{
		return false;
	}

	return true;
}
bool CGameLogic::judgeMust(const _uint8 mustOutCard[], _uint8 mustOutCnt, _uint8 cbOutCards[], _uint8 nOutCnt)
{
	_uint8 cnt = 0;
	for (_uint8 i = 0; i < mustOutCnt; ++i)
	{
		cnt = 0;
		for (_uint8 j = 0; j < nOutCnt; ++j)
		{
			if (mustOutCard[i] == cbOutCards[j])
			{
				cbOutCards[j] = 0;
				cnt++;
			}
		}
		if (0 == cnt)
		{
			return false;
		}
	}
	return true;
}

bool CGameLogic::judgeCan(const _uint8 canOutCard[], _uint8 canOutCnt, _uint8 cbOutCards[], _uint8 nOutCnt, const _uint8 cbCards[PLAYER_CARD_CNT], _uint8 nCardCnt)
{
	if (0 == canOutCnt)
	{
		return true;
	}
	_uint8 cnt = 0;
	for (_uint8 i = 0; i < canOutCnt; ++i)
	{
		for (_uint8 j = 0; j < nOutCnt; ++j)
		{
			if (canOutCard[i] == cbOutCards[j])
			{

				if (0 == GetCardCnt(canOutCard[i], cbOutCards, nOutCnt))
				{
					return false;
				}
				else
				{
					cnt++;
					cbOutCards[j] = 0;
				}
			}
		}
	}

	return true;
}
bool CGameLogic::judgeClr(const _uint8 canOutClr[], _uint8 canOutClrCnt, _uint8 cbOutCards[], _uint8 nOutCnt)
{
	if (0 == canOutClrCnt)
	{
		return true;
	}
	for (_uint8 j = 0; j < nOutCnt; ++j)
	{
		if (0 == cbOutCards[j])
		{
			continue;
		}
		_uint8 i = 0;
		for (; i < canOutClrCnt; ++i)
		{
			if (canOutClr[i] == GetCardColor(cbOutCards[j]))
			{
				break;
			}
		}
		if (i >= canOutClrCnt)
		{
			return false;
		}
	}
	return true;
}

_uint8 CGameLogic::GetRepFan(_uint8 cbRepCards[], _uint8 nCount)
{
	bool bSame = false;
	bool bDB = false;
	bool bTK = false;

	SortCard(cbRepCards, nCount);

	for (_uint8 i = 0; i < nCount - 1; ++i)
	{
		if (!bSame)
		{
			_uint8 db[PLAYER_CARD_CNT / 2]; //最多PLAYER_CARD_CNT/2个对子
			_uint8 dbCnt = 0;

			GetAllDBByClr(cbRepCards, nCount, NULL, NULL, db, &dbCnt, NULL, NULL, cbRepCards[i]);

			if (0 < dbCnt)
			{
				_uint8 Lps_tkCnt = 0;
				_uint8 Lps_start[COLOR_CARD_CNT / 2];  //起始位置
				_uint8 Lps_length[COLOR_CARD_CNT / 2]; //拖拉机长度
				GetTracker(db, dbCnt, Lps_start, Lps_length, Lps_tkCnt);
				if (Lps_tkCnt > 0)
				{
					bTK = true;
				}
				else
				{
					bDB = true;
				}
			}
			bSame = true;
		}
		if (!SameClr(cbRepCards[i], cbRepCards[i + 1]))
		{
			bSame = false;
		}
	}

	return bTK ? 16 : (bDB ? 4 : 2);
}