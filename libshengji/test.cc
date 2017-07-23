#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "gameLogic.h"

int main(int argc, char const *argv[])
{
	CGameLogic m_gameLogic;
	//int maxTrackorCnt = 0;
	_uint8 m_cbAllCards[TOTAL_CARD_CNT];
	_uint8 m_cbHandCards[GAME_PLAYER][PLAYER_CARD_CNT];
	_uint8 m_cbBackCards[BACK_CARD_CNT];
	_uint8 m_nHandCnt[GAME_PLAYER];
	memset(m_nHandCnt, PLAYER_CARD_CNT, sizeof(m_nHandCnt));
	m_gameLogic.RandCard(m_cbAllCards);
	for (int i = 0; i < GAME_PLAYER; ++i){
		memcpy(m_cbHandCards[i], &m_cbAllCards[i*PLAYER_CARD_CNT], sizeof(_uint8) * PLAYER_CARD_CNT);
	}
	memcpy(m_cbBackCards, &m_cbAllCards[GAME_PLAYER*PLAYER_CARD_CNT], sizeof(_uint8) * BACK_CARD_CNT);
	for(int j = 0; j < BACK_CARD_CNT; ++j)
	{
		fprintf(stdout, "0x%02x ", m_cbBackCards[j]);
	}
	fprintf(stdout, "\n");
	//m_gameLogic.SetNTColor(2);
	

	for (int i = 0; i < GAME_PLAYER; ++i){
		m_gameLogic.SortCard(m_cbHandCards[i], PLAYER_CARD_CNT);
		fprintf(stdout, "player[%d] cards[[", i);
		for(int j = 0; j < PLAYER_CARD_CNT; ++j){
			fprintf(stdout, "0x%02x ", m_cbHandCards[i][j]);
		}
		fprintf(stdout, "]] \n");
		/*
		for(int j = 0; j < 5; j++){
			_uint8 nColor = j << 4;
			_uint8 cbSameColorCards[13];
			_uint8 nSameColorCnt = m_gameLogic.TakeOutCardByColor(m_cbHandCards[i], m_nHandCnt[i], nColor, cbSameColorCards, true);
			fprintf(stdout, "color[0x%02x] cards[[", nColor);
			for (int k = 0; k < nSameColorCnt; ++k){
				fprintf(stdout, "0x%02x ", cbSameColorCards[k]);
			}
			fprintf(stdout, "]] \n");
			tagCardAnalyse tCardAna;
			m_gameLogic.AnalyseCard(cbSameColorCards, nSameColorCnt, tCardAna);
			printf("player[%d]:牌的花色[0x%02x] 单牌数量[%d] 最小单牌[0x%02x] 最大单牌[0x%02x] 对子数量[%d] 拖拉机数量[%d]\n", 	
					i, tCardAna.nCardColor, tCardAna.nSingleCnt, tCardAna.nSmallSingleCard, tCardAna.nBigSingleCard,tCardAna.nDoubleCnt, tCardAna.nTrackorCnt);
			m_nHandCnt[i] -= nSameColorCnt;
			maxTrackorCnt = tCardAna.nTrackorCnt;
		}*/
	}
	bool tb[2] = {false, true};

	for (int m = 0; m < 2; ++m)
	{
		for (int n = 0; n < 2; ++n)
		{
			for (int j = 0; j < 2; ++j)
			{
				for (int v = 0; v < 2; ++v)
				{
					for (int k = 1; k <= 13; ++k)
					{
						for (int i = 0; i < MAX_INDEX; ++i)
						{
							if (i % 13 == 0)
							{
								fprintf(stdout, "\n");
							}
							fprintf(stdout, "%2d ", m_gameLogic.GetSortWeight(g_cbCards[i], k, tb[v], tb[m], tb[j], tb[n]));
						}
						fprintf(stdout, "\n");
					}
				}
			}
		}
	}
	return 0;

	fprintf(stdout, "\n");

#define MAX 9
	_uint8 cards1[MAX][14] ={
		{0x01},
	 	{0x01,0x01, 0x0d, 0x0d},
		{0x01,0x01, 0x0c, 0x0c},
	 	{0x01,0x01, 0x0d, 0x0d,0x0c, 0x0c},
		{0x01,0x01, 0x0d, 0x0d,0x0b, 0x0b},
	 	{0x21,0x2d,0x2c},
	 	{0x21,0x21},
	 	{0x21,0x01},
		{0x01,0x01, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x08, 0x08},
		};
	_uint8 cnt[]=
	{
		1,
		4,
		4,
		6,
		6,
		3,
		2,
		2,
		14,
	};
	for (int i = 0; i < MAX; ++i)
	{
		switch (m_gameLogic.GetCardType(cards1[i], cnt[i]))
		{
			case TYPE_SINGLE:
			{
				fprintf(stdout, "[%d] TYPE_SINGLE\n",i);
			}break;
			case TYPE_DOUBLE:
			{
				fprintf(stdout, "[%d] TYPE_DOUBLE\n",i);
			}break;
			case TYPE_TRACKER:
			{
				fprintf(stdout, "[%d] TYPE_TRACKER\n",i);
			}break;
			case TYPE_FLING:
			{
				fprintf(stdout, "[%d] TYPE_FLING\n",i);
			}break;
			case TYPE_FLING_DB:
			{
				fprintf(stdout, "[%d] TYPE_FLING_DB\n",i);
			}break;
			default:
			{
				fprintf(stdout, "[%d] TYPE_ERROR\n",i);
			}break;
				break;
		}
	}

	_uint8 mustOutCard[PLAYER_CARD_CNT];
	_uint8 mustOutCnt;
	_uint8 canOutCard[PLAYER_CARD_CNT];
	_uint8 canOutCnt;
	_uint8 canOutClr[5];
	_uint8 canOutClrCnt;

	int j = 0;
	for (; j < GAME_PLAYER; ++j)
	{
		fprintf(stdout, "player[%d] cards[[", j);
		for (int k = 0; k < PLAYER_CARD_CNT; ++k)
		{
			fprintf(stdout, "0x%02x ", m_cbHandCards[j][k]);
		}
		fprintf(stdout, "]] \n");
		for (int i = 0; i < MAX; ++i)
		{
			for (int k = 0; k < cnt[i]; ++k)
			{
				fprintf(stdout, "[%#x] ", cards1[i][k]);
			}

			m_gameLogic.GetOutCards(cards1[i], cnt[i], m_cbHandCards[j], PLAYER_CARD_CNT, mustOutCard, mustOutCnt, canOutCard, canOutCnt, canOutClr, canOutClrCnt);

			fprintf(stdout, "\nmust");
			for (int k = 0; k < mustOutCnt; ++k)
			{
				fprintf(stdout, "[%#x] ", mustOutCard[k]);
			}
			fprintf(stdout, "\ncan");
			for (int k = 0; k < canOutCnt; ++k)
			{
				fprintf(stdout, "[%#x] ", canOutCard[k]);
			}
			fprintf(stdout, "\nclr");
			for (int k = 0; k < canOutClrCnt; ++k)
			{
				fprintf(stdout, "[%#x] ", canOutClr[k]);
			}
			fprintf(stdout, "\n");
		}
	}

	m_gameLogic.SetNTCard(0x02);

	_uint8 cbHandCards[GAME_PLAYER][PLAYER_CARD_CNT] = {
		{0x41,0x41,0x02,0x02,0x0d,0x0a,0x0a},
		{0x42,0x02,0x12,0x2c,0x2c},
		{0x42,0x32,0x32,0x01,0x2b},
		{0x22,0x22,0x02,0x01,0x0d},
	};
	
	_uint8 nHandCnt[GAME_PLAYER] = { 7,5,5,5} ;

	_uint8 outcard[PLAYER_CARD_CNT] = {0x42,0x01,0x01};

	_uint8 res[PLAYER_CARD_CNT];
	_uint8 rescnt = 0;
	_uint8 type = 0;

	fprintf(stdout, "\n\n\n\n\n");

	m_gameLogic.CanFlingCard(outcard, 3, 1, cbHandCards, GAME_PLAYER, nHandCnt, res, rescnt, type);

	switch (type)
	{
	case TYPE_SINGLE:
	{
		fprintf(stdout, "TYPE_SINGLE\n");
	}
	break;
	case TYPE_DOUBLE:
	{
		fprintf(stdout, "TYPE_DOUBLE\n");
	}
	break;
	case TYPE_TRACKER:
	{
		fprintf(stdout, "TYPE_TRACKER\n");
	}
	break;
	case TYPE_FLING:
	{
		fprintf(stdout, "TYPE_FLING\n");
	}
	break;
	case TYPE_FLING_DB:
	{
		fprintf(stdout, "TYPE_FLING_DB\n");
	}
	break;
	default:
	{
		fprintf(stdout, "TYPE_ERROR\n");
	}
	break;
		break;
	}

	for (int k = 0; k < rescnt; ++k)
	{
		fprintf(stdout, "[%#x] ", res[k]);
	}

	fprintf(stdout, "\n");


	_uint8 cbOutCards[6][7] = {
		{0x42,0x42,0x41,0x41,0x0d,0x09,0x09},
		{0x41,0x41,0x02,0x02,0x0d,0x0a,0x0a},
		{0x41,0x41,0x02,0x01,0x0d},
		{0x42,0x42,0x41,0x01,0x02},
		{0x41,0x02,0x01,0x0d},
		{0x42,0x41,0x01,0x02},
	};
	_uint8 cbOutCardsL[3] = {7,5,4};
	for (int i = 0; i < 6; i+=2)
	{
		
		fprintf(stdout, "[%d] ", m_gameLogic.CompareOutCard(cbOutCards[i], cbOutCards[i + 1], cbOutCardsL[i/2]));
	}

	fprintf(stdout, "\n");
	return 0;
}