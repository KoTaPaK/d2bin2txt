#include "global.h"

//#define USE_NUMBER
#define EQUAL_OPER_BRACKET

#define FILE_PREFIX "misscode"
#define STACK_HEAD  4

typedef struct
{
    unsigned char acExpressionTxt[10240];
    unsigned char acExpressionBin[10240];
    unsigned int uiBinLen;
} ST_MISSCODE;

static void *m_pvStack = NULL;
static unsigned int m_uiExpressionIndex = 0;
static unsigned int m_uiMissCodeCount = 0;
static ST_MISSCODE m_astMissCode[5000];

typedef int (*fnProcFuncArg)(unsigned char *pcBinBuf, unsigned char *pcTxtBuf);

static void MissCode_PrintHex()
{
    unsigned int j;

    for ( j = 0; j < m_uiExpressionIndex; j++ )
    {
        my_printf("%02x ", (unsigned char)m_acGlobalBuffer[j]);
    }
    my_printf("\r\n");
}

#define FINISH_CODE_TAG 0xFF

static int MissCode_ConverterBin2Txt(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
            pcResult = MissCalc_GetCalcCode(pcBinBuf[STACK_HEAD]);
            if ( pcResult )
            {
                strcpy(pcTxtBuf, pcResult);
            }
            else
            {
                my_error("unknown miss calc\r\n");
                result = 0;
            }
            break;

        case 0x07:
            sprintf(pcTxtBuf, "%d", (char)pcBinBuf[STACK_HEAD]);
            break;

        case 0x08:
            sprintf(pcTxtBuf, "%d", *(short *)&pcBinBuf[STACK_HEAD]);
            break;

        case 0x09:
            sprintf(pcTxtBuf, "%d", *(int *)&pcBinBuf[STACK_HEAD]);
            break;

        case FINISH_CODE_TAG:
            strcpy(pcTxtBuf, &pcBinBuf[STACK_HEAD]);
            break;

        default:
            my_error("unknown binary\r\n");
            result = 0;
            break;
    }

    return result;
}

static int MissCode_SkillArg1Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = Skills_GetSkillName(uiSkillIndex);

        if ( pcResult)
        {
            sprintf(pcTxtBuf, "'%s'", pcResult);
        }
        else
        {
            my_error("not found miss %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_SkillArg2Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
        pcResult = SkillCalc_GetCalc(uiSkillIndex);

        if ( pcResult)
        {
            strcpy(pcTxtBuf, pcResult);
        }
        else
        {
            my_error("not found misscalc %u\r\n", uiSkillIndex);
            result = 0;
        }
    }

    return result;
}

static int MissCode_StatArg1Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            MissCode_PrintHex();
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = ItemStatCost_GetStateName(uiSkillIndex);

        if ( pcResult)
        {
            sprintf(pcTxtBuf, "'%s'", pcResult);
        }
        else
        {
            my_error("not found itemstatcost %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_StatArg2Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
        if ( 0 == uiSkillIndex )
        {
            strcpy(pcTxtBuf, "accr");
        }
        else if ( 1 == uiSkillIndex )
        {
            strcpy(pcTxtBuf, "base");
        }
        else if ( 2 == uiSkillIndex )
        {
            strcpy(pcTxtBuf, "mod");
        }
        else
        {
            my_error("invalide itemstatcost %u\r\n", uiSkillIndex);
            MissCode_PrintHex();
            result = 0;
        }
    }

    return result;
}

static int MissCode_MissArg1Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = Missiles_GetMissile(uiSkillIndex);

        if ( pcResult)
        {
            sprintf(pcTxtBuf, "'%s'", pcResult);
        }
        else
        {
            my_error("not found miss %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_MissArg2Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown miss arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = MissCalc_GetCalcCode(uiSkillIndex);

        if ( pcResult)
        {
            strcpy(pcTxtBuf, pcResult);
        }
        else
        {
            my_error("not found misscalc %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_SkLvlArg1Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown skill arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = Skills_GetSkillName(uiSkillIndex);

        if ( pcResult)
        {
            sprintf(pcTxtBuf, "'%s'", pcResult);
        }
        else
        {
            my_error("not found skill %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_SkLvlArg2Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown skillcalc arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = SkillCalc_GetCalc(uiSkillIndex);

        if ( pcResult)
        {
            strcpy(pcTxtBuf, pcResult);
        }
        else
        {
            my_error("not found skillcalc %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_SkLvlArg3Proc(unsigned char *pcBinBuf, unsigned char *pcTxtBuf)
{
    int result = 1;
    unsigned int uiSkillIndex;
    unsigned char *pcResult;

    switch ( pcBinBuf[0] )
    {
        case 0x04:
        case 0x07:
            uiSkillIndex = (unsigned char)pcBinBuf[STACK_HEAD];
            break;

        case 0x08:
            uiSkillIndex = *(unsigned short *)&pcBinBuf[STACK_HEAD];
            break;

        case 0x09:
            uiSkillIndex = *(unsigned int *)&pcBinBuf[STACK_HEAD];
            break;

        default:
            my_error("unknown skillcalc arg 1\r\n");
            result = 0;
            break;
    }

    if ( 0 != result )
    {
#ifdef USE_NUMBER
        sprintf(pcTxtBuf, "%u", uiSkillIndex);
#else
        pcResult = SkillCalc_GetCalc(uiSkillIndex);

        if ( pcResult)
        {
            strcpy(pcTxtBuf, pcResult);
        }
        else
        {
            my_error("not found skillcalc %u\r\n", uiSkillIndex);
            result = 0;
        }
#endif
    }

    return result;
}

static int MissCode_OutputExpression(unsigned char *pcTempBuf)
{
    int result = 1;

    result = MissCode_ConverterBin2Txt(pcTempBuf, m_astMissCode[m_uiMissCodeCount].acExpressionTxt);

    if ( 0 != result )
    {
        memcpy(m_astMissCode[m_uiMissCodeCount].acExpressionBin, m_acGlobalBuffer, m_uiExpressionIndex);
        m_astMissCode[m_uiMissCodeCount].uiBinLen = m_uiExpressionIndex;
        m_uiExpressionIndex = 0;
        m_uiMissCodeCount++;
    }

    return result;
}

static int MissCode_ReadFile(unsigned char *pcBuffer, int iElemSize, int iElemCount, FILE *pfHandle)
{
    int iReadByte = fread(pcBuffer, iElemSize, iElemCount, pfHandle);

    if ( iReadByte == iElemCount * iElemSize )
    {
        memcpy(&m_acGlobalBuffer[m_uiExpressionIndex], pcBuffer, iReadByte);
        m_uiExpressionIndex += iReadByte;

        return iReadByte;
    }

    return 0;
}

static int MissCode_OneOprandProc(unsigned char *pbOperater, unsigned int uiOpLevel, ENUM_OPERATER_ARGNUM enArgNum,
    ENUM_OPERATER_LEFTRIGHT enLeftRight, unsigned char *acExpressionBuf, unsigned int uiBufLen)
{
    int result = 1;
    unsigned char *pcOperand1 = NULL;

    Stack_Pop(m_pvStack, &pcOperand1);

    acExpressionBuf[0] = FINISH_CODE_TAG;
    acExpressionBuf[1] = (unsigned char)uiOpLevel;
    acExpressionBuf[2] = FINISH_CODE_TAG;
    acExpressionBuf[3] = FINISH_CODE_TAG;

#ifdef EQUAL_OPER_BRACKET
    if ( uiOpLevel <= pcOperand1[1] )
#else
    if ( uiOpLevel < pcOperand1[1] || 
        (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
    {
        sprintf(&acExpressionBuf[strlen(acExpressionBuf)], "%s(", pbOperater);
    }
    else
    {
        sprintf(&acExpressionBuf[strlen(acExpressionBuf)], "%s", pbOperater);
    }

    if ( 0 == MissCode_ConverterBin2Txt(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]) )
    {
        my_error("operater %s fail\r\n", pbOperater);
        result = 0;
    }
    else
    {
#ifdef EQUAL_OPER_BRACKET
        if ( uiOpLevel <= pcOperand1[1] )
#else
        if ( uiOpLevel < pcOperand1[1] || 
            (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
        {
            acExpressionBuf[strlen(acExpressionBuf)] = ')';
        }
        Stack_Push(m_pvStack, acExpressionBuf, strlen(acExpressionBuf));
    }

    return result;
}

static int MissCode_TwoOprandProc(unsigned char *pbOperater, unsigned int uiOpLevel, ENUM_OPERATER_ARGNUM enArgNum,
    ENUM_OPERATER_LEFTRIGHT enLeftRight, unsigned char *acExpressionBuf, unsigned int uiBufLen)
{
    int result = 1;
    unsigned char *pcOperand1 = NULL;
    unsigned char *pcOperand2 = NULL;

    Stack_Pop(m_pvStack, &pcOperand2);
    Stack_Pop(m_pvStack, &pcOperand1);

    acExpressionBuf[0] = FINISH_CODE_TAG;
    acExpressionBuf[1] = (unsigned char)uiOpLevel;
    acExpressionBuf[2] = FINISH_CODE_TAG;
    acExpressionBuf[3] = FINISH_CODE_TAG;

#ifdef EQUAL_OPER_BRACKET
    if ( uiOpLevel <= pcOperand1[1] )
#else
    if ( uiOpLevel < pcOperand1[1] || 
        (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
    {
        acExpressionBuf[strlen(acExpressionBuf)] = '(';
    }

    if ( 0 == MissCode_ConverterBin2Txt(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]) )
    {
        my_error("operater %s fail\r\n", pbOperater);
        result = 0;
    }
    else
    {
#ifdef EQUAL_OPER_BRACKET
        if ( uiOpLevel <= pcOperand1[1] )
#else
        if ( uiOpLevel < pcOperand1[1] || 
            (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
        {
            sprintf(&acExpressionBuf[strlen(acExpressionBuf)], ")%s", pbOperater);
        }
        else
        {
            sprintf(&acExpressionBuf[strlen(acExpressionBuf)], "%s", pbOperater);
        }

#ifdef EQUAL_OPER_BRACKET
        if ( uiOpLevel <= pcOperand2[1] )
#else
        if ( uiOpLevel < pcOperand2[1] || 
            (uiOpLevel == pcOperand2[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
        {
            acExpressionBuf[strlen(acExpressionBuf)] = '(';
        }

        if ( 0 == MissCode_ConverterBin2Txt(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]) )
        {
            my_error("operater %s fail 2\r\n", pbOperater);
            result = 0;
        }
        else
        {
#ifdef EQUAL_OPER_BRACKET
            if ( uiOpLevel <= pcOperand2[1] )
#else
            if ( uiOpLevel < pcOperand2[1] || 
                (uiOpLevel == pcOperand2[1] && EN_OPERATER_RIGHT == enLeftRight) )
#endif
            {
                acExpressionBuf[strlen(acExpressionBuf)] = ')';
            }
            Stack_Push(m_pvStack, acExpressionBuf, strlen(acExpressionBuf));
        }
    }

    return result;
}

static int MissCode_ThreeOprandProc(unsigned char *pbOperater1, unsigned char *pbOperater2, unsigned int uiOpLevel, 
    ENUM_OPERATER_ARGNUM enArgNum, ENUM_OPERATER_LEFTRIGHT enLeftRight, unsigned char *acExpressionBuf, unsigned int uiBufLen)
{
    int result = 1;
    unsigned char *pcOperand1 = NULL;
    unsigned char *pcOperand2 = NULL;
    unsigned char *pcOperand3 = NULL;

    Stack_Pop(m_pvStack, &pcOperand3);
    Stack_Pop(m_pvStack, &pcOperand2);
    Stack_Pop(m_pvStack, &pcOperand1);

    acExpressionBuf[0] = FINISH_CODE_TAG;
    acExpressionBuf[1] = (unsigned char)uiOpLevel;
    acExpressionBuf[2] = FINISH_CODE_TAG;
    acExpressionBuf[3] = FINISH_CODE_TAG;

#if 0
    if ( uiOpLevel < pcOperand1[1] || 
        (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
    if ( 1 < pcOperand1[1] )
#endif
    {
        acExpressionBuf[strlen(acExpressionBuf)] = '(';
    }

    if ( 0 == MissCode_ConverterBin2Txt(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]) )
    {
        my_error("operater %s fail\r\n", pbOperater1);
        result = 0;
    }
    else
    {
#if 0
        if ( uiOpLevel < pcOperand1[1] || 
            (uiOpLevel == pcOperand1[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
        if ( 1 < pcOperand1[1] )
#endif
        {
            acExpressionBuf[strlen(acExpressionBuf)] = ')';
            acExpressionBuf[strlen(acExpressionBuf)] = *pbOperater1;
        }
        else
        {
            acExpressionBuf[strlen(acExpressionBuf)] = *pbOperater1;
        }

#if 0
        if ( uiOpLevel < pcOperand2[1] || 
            (uiOpLevel == pcOperand2[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
        if ( 1 < pcOperand2[1] )
#endif
        {
            acExpressionBuf[strlen(acExpressionBuf)] = '(';
        }

        if ( 0 == MissCode_ConverterBin2Txt(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]) )
        {
            my_error("operater %s fail 2\r\n", pbOperater1);
            result = 0;
        }
        else
        {
#if 0
            if ( uiOpLevel < pcOperand2[1] || 
                (uiOpLevel == pcOperand2[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
            if ( 1 < pcOperand2[1] )
#endif
            {
                acExpressionBuf[strlen(acExpressionBuf)] = ')';
                acExpressionBuf[strlen(acExpressionBuf)] = *pbOperater2;
            }
            else
            {
                acExpressionBuf[strlen(acExpressionBuf)] = *pbOperater2;
            }

#if 0
            if ( uiOpLevel < pcOperand3[1] || 
                (uiOpLevel == pcOperand3[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
            if ( 1 < pcOperand3[1] )
#endif
            {
                acExpressionBuf[strlen(acExpressionBuf)] = '(';
            }

            if ( 0 == MissCode_ConverterBin2Txt(pcOperand3, &acExpressionBuf[strlen(acExpressionBuf)]) )
            {
                my_error("operater %s fail 3\r\n", pbOperater2);
                result = 0;
            }
            else
            {
#if 0
                if ( uiOpLevel < pcOperand3[1] || 
                    (uiOpLevel == pcOperand3[1] && EN_OPERATER_RIGHT == enLeftRight) )
#else
                if ( 1 < pcOperand3[1] )
#endif
                {
                    acExpressionBuf[strlen(acExpressionBuf)] = ')';
                }
                Stack_Push(m_pvStack, acExpressionBuf, strlen(acExpressionBuf));
            }
        }
    }

    return result;
}

static int MissCode_TwoArgFuncProc(unsigned char *pbFunction, unsigned char *pbOperater, 
    unsigned char *acExpressionBuf, unsigned int uiBufLen,
    fnProcFuncArg pfnProcArg1, fnProcFuncArg pfnProcArg2)
{
    int result = 1;
    unsigned char *pcOperand1 = NULL;
    unsigned char *pcOperand2 = NULL;

    Stack_Pop(m_pvStack, &pcOperand2);
    Stack_Pop(m_pvStack, &pcOperand1);

    acExpressionBuf[0] = FINISH_CODE_TAG;
    acExpressionBuf[1] = FUNCTION_OPERATER_LEVEL;
    acExpressionBuf[2] = FINISH_CODE_TAG;
    acExpressionBuf[3] = FINISH_CODE_TAG;

    sprintf(&acExpressionBuf[strlen(acExpressionBuf)], "%s(", pbFunction);
    if ( (pfnProcArg1 && 1 == pfnProcArg1(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]))
        || 1 == MissCode_ConverterBin2Txt(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]) )
    {
        strcpy(&acExpressionBuf[strlen(acExpressionBuf)], pbOperater);

        if ( (pfnProcArg2 && 1 == pfnProcArg2(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]))
            || 1 == MissCode_ConverterBin2Txt(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]) )
        {
            acExpressionBuf[strlen(acExpressionBuf)] = ')';
            Stack_Push(m_pvStack, acExpressionBuf, strlen(acExpressionBuf));
        }
        else
        {
            my_error("operater %s fail 2\r\n", pbOperater);
            result = 0;
        }
    }
    else
    {
        my_error("operater %s fail\r\n", pbOperater);
        result = 0;
    }

    return result;
}

static int MissCode_ThreeArgFuncProc(unsigned char *pbFunction, unsigned char *pbOperater1, unsigned char *pbOperater2,
    unsigned char *acExpressionBuf, unsigned int uiBufLen,
    fnProcFuncArg pfnProcArg1, fnProcFuncArg pfnProcArg2, fnProcFuncArg pfnProcArg3)
{
    int result = 1;
    unsigned char *pcOperand1 = NULL;
    unsigned char *pcOperand2 = NULL;
    unsigned char *pcOperand3 = NULL;

    Stack_Pop(m_pvStack, &pcOperand3);
    Stack_Pop(m_pvStack, &pcOperand2);
    Stack_Pop(m_pvStack, &pcOperand1);

    acExpressionBuf[0] = FINISH_CODE_TAG;
    acExpressionBuf[1] = FUNCTION_OPERATER_LEVEL;
    acExpressionBuf[2] = FINISH_CODE_TAG;
    acExpressionBuf[3] = FINISH_CODE_TAG;

    sprintf(&acExpressionBuf[strlen(acExpressionBuf)], "%s(", pbFunction);

    if ( (pfnProcArg1 && 1 == pfnProcArg1(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]))
        || 1 == MissCode_ConverterBin2Txt(pcOperand1, &acExpressionBuf[strlen(acExpressionBuf)]) )
    {
        strcpy(&acExpressionBuf[strlen(acExpressionBuf)], pbOperater1);

        if ( (pfnProcArg2 && 1 == pfnProcArg2(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]))
            || 1 == MissCode_ConverterBin2Txt(pcOperand2, &acExpressionBuf[strlen(acExpressionBuf)]) )
        {
            strcpy(&acExpressionBuf[strlen(acExpressionBuf)], pbOperater2);

            if ( (pfnProcArg3 && 1 == pfnProcArg3(pcOperand3, &acExpressionBuf[strlen(acExpressionBuf)]))
                || 1 == MissCode_ConverterBin2Txt(pcOperand3, &acExpressionBuf[strlen(acExpressionBuf)]) )
            {
                acExpressionBuf[strlen(acExpressionBuf)] = ')';
                Stack_Push(m_pvStack, acExpressionBuf, strlen(acExpressionBuf));
            }
            else
            {
                my_error("operater %s fail 3\r\n", pbOperater2);
                result = 0;
            }
        }
        else
        {
            my_error("operater %s fail 2\r\n", pbOperater2);
            result = 0;
        }
    }
    else
    {
        my_error("operater %s fail\r\n", pbOperater1);
        result = 0;
    }

    return result;
}

static int MissCode_OperaterProc(unsigned char bOperater)
{
    int result = 1;
    unsigned int uiOpLevel;
    unsigned char *pcOpString;
    ENUM_OPERATER_ARGNUM enArgNum;
    ENUM_OPERATER_LEFTRIGHT enLeftRight;
    static unsigned char acExpressionBuf[10240];

    uiOpLevel = Operater_GetOpLevel((ENUM_OPERATER_TYPE)bOperater);
    pcOpString = Operater_GetOpStr((ENUM_OPERATER_TYPE)bOperater);
    enArgNum = Operater_GetArgNum((ENUM_OPERATER_TYPE)bOperater);
    enLeftRight = Operater_GetLeftRight((ENUM_OPERATER_TYPE)bOperater);

    if ( MAX_OPERATER_LEVEL == uiOpLevel )
    {
        my_error("unknown tag %u\r\n", bOperater);
        MissCode_PrintHex();
        return result;
    }

    memset(acExpressionBuf, 0, sizeof(acExpressionBuf));

    switch ( enArgNum )
    {
        case EN_OPERATER_ARG_1:
            result = MissCode_OneOprandProc(pcOpString, uiOpLevel, enArgNum, enLeftRight, acExpressionBuf, sizeof(acExpressionBuf));
            break;

        case EN_OPERATER_ARG_2:
            result = MissCode_TwoOprandProc(pcOpString, uiOpLevel, enArgNum, enLeftRight, acExpressionBuf, sizeof(acExpressionBuf));
            break;

        case EN_OPERATER_ARG_3:
            result = MissCode_ThreeOprandProc(&pcOpString[0], &pcOpString[1], uiOpLevel, enArgNum, enLeftRight, acExpressionBuf, sizeof(acExpressionBuf));
            break;

        default:
            my_error("unknown argnum %u\r\n", bOperater);
            MissCode_PrintHex();
            return result;
            break;
    }

    return result;
}

int MissCode_ParseBin(char *acTemplatePath, char *acBinPath, char*acTxtPath)
{
    int result = 1;
    unsigned int i, j;
    FILE *pfBinHandle = NULL;
    FILE *pfOutputHandle = NULL;
    unsigned char bOperater;
    unsigned char *pcTemp;
    static unsigned char acTempBuf[10240] = {0};

    if ( 0 < m_uiMissCodeCount )
    {
        return 1;
    }

    my_printf("start parsing %s.bin\r\n", FILE_PREFIX);

    MODULE_DEPEND_CALL(skills, acTemplatePath, acBinPath, acTxtPath);
    MODULE_DEPEND_CALL(skillcalc, acTemplatePath, acBinPath, acTxtPath);
    MODULE_DEPEND_CALL(itemstatcost, acTemplatePath, acBinPath, acTxtPath);
    MODULE_DEPEND_CALL(missiles, acTemplatePath, acBinPath, acTxtPath);
    MODULE_DEPEND_CALL(misscalc, acTemplatePath, acBinPath, acTxtPath);

    m_uiExpressionIndex = 0;
    m_uiMissCodeCount = 0;
    memset(m_astMissCode, 0, sizeof(m_astMissCode));

    memset(m_acGlobalBuffer, 0, m_iGlobaBufLength);
    sprintf(m_acGlobalBuffer, "%s\\%s.bin", acBinPath, FILE_PREFIX);
    pfBinHandle = fopen(m_acGlobalBuffer, "rb");

    memset(m_acGlobalBuffer, 0, m_iGlobaBufLength);
    sprintf(m_acGlobalBuffer, "%s\\%s.txt", acTxtPath, FILE_PREFIX);
    pfOutputHandle = fopen(m_acGlobalBuffer, "wb");

    if ( NULL == pfBinHandle || NULL == pfOutputHandle )
    {
        my_error("failed to read bin\r\n");
        goto error;
    }

    memset(m_acGlobalBuffer, 0, m_iGlobaBufLength);
    memset(acTempBuf, 0, sizeof(acTempBuf));

    if ( NULL == ( m_pvStack = Stack_Create(sizeof(acTempBuf), 500)) )
    {
        my_error("failed to create stack!\r\n");
        goto error;
    }

    while ( 1 == MissCode_ReadFile(acTempBuf, 1, 1, pfBinHandle) )
    {
        acTempBuf[1] = SINGLE_OPERATER_LEVEL;
        switch ( acTempBuf[0] )
        {
            case 0x04:
            case 0x07:
                //byte型数据
                if ( 1 != MissCode_ReadFile(&acTempBuf[STACK_HEAD], 1, sizeof(unsigned char), pfBinHandle) )
                {
                    my_error("invalid tag %x\r\n", acTempBuf[0]);
                    goto error;
                }
                Stack_Push(m_pvStack, acTempBuf, STACK_HEAD + sizeof(unsigned char));
                break;

            case 0x08:
                //short型数据
                if ( 2 != MissCode_ReadFile(&acTempBuf[STACK_HEAD], 1, sizeof(unsigned short), pfBinHandle) )
                {
                    my_error("invalid tag %x\r\n", acTempBuf[0]);
                    goto error;
                }
                Stack_Push(m_pvStack, acTempBuf, STACK_HEAD + sizeof(unsigned short));
                break;

            case 0x09:
                //int型数据
                if ( 4 != MissCode_ReadFile(&acTempBuf[STACK_HEAD], 1, sizeof(unsigned int), pfBinHandle) )
                {
                    my_error("invalid tag %x\r\n", acTempBuf[0]);
                    goto error;
                }
                Stack_Push(m_pvStack, acTempBuf, STACK_HEAD + sizeof(unsigned int));
                break;

            case 0x00:
                //表达式结束
                Stack_Pop(m_pvStack, &pcTemp);

                if ( 0 == Stack_IsEmpty(m_pvStack) )
                {
                    MissCode_PrintHex();
                }

                while ( 0 == Stack_IsEmpty(m_pvStack) )
                {
                    my_error("invalid expression\r\n");
                    Stack_Pop(m_pvStack, &pcTemp);
                    //goto error;
                }

                if ( 0 == MissCode_OutputExpression(pcTemp) )
                {
                    my_error("save expression fail\r\n");
                    goto error;
                }
                break;

            case 0x01:
                if ( 1 != MissCode_ReadFile(&acTempBuf[1], 1, 1, pfBinHandle) )
                {
                    my_error("invalid tag %x\r\n", acTempBuf[0]);
                    goto error;
                }

                result = 0;
                bOperater = acTempBuf[1];
                memset(acTempBuf, 0, sizeof(acTempBuf));
                if ( 0x03 == bOperater )
                {
                    //skill(xxx)
                    result = MissCode_TwoArgFuncProc("skill", ".", acTempBuf, sizeof(acTempBuf), 
                        MissCode_SkillArg1Proc, MissCode_SkillArg2Proc);
                }
                else if ( 0x00 == bOperater )
                {
                    //min(xxx)
                    result = MissCode_TwoArgFuncProc("min", ",", acTempBuf, sizeof(acTempBuf), NULL, NULL);
                }
                else if ( 0x01 == bOperater )
                {
                    //max(xxx)
                    result = MissCode_TwoArgFuncProc("max", ",", acTempBuf, sizeof(acTempBuf), NULL, NULL);
                }
                else if ( 0x05 == bOperater )
                {
                    //stat(xxx)
                    result = MissCode_TwoArgFuncProc("stat", ".", acTempBuf, sizeof(acTempBuf), 
                        MissCode_StatArg1Proc, MissCode_StatArg2Proc);
                }
                else if ( 0x02 == bOperater )
                {
                    //rand(xxx)
                    result = MissCode_TwoArgFuncProc("rand", ",", acTempBuf, sizeof(acTempBuf), NULL, NULL);
                }
                else if ( 0x04 == bOperater )
                {
                    //miss(xxx)
                    result = MissCode_TwoArgFuncProc("miss", ".", acTempBuf, sizeof(acTempBuf), 
                        MissCode_MissArg1Proc, MissCode_MissArg2Proc);
                }
                else if ( 0x06 == bOperater )
                {
                    //sklvl(xxx)
                    result = MissCode_ThreeArgFuncProc("sklvl", ".", ".", acTempBuf, sizeof(acTempBuf), 
                        MissCode_SkLvlArg1Proc, MissCode_SkLvlArg2Proc, MissCode_SkLvlArg3Proc);
                }

                if ( 0 == result )
                {
                    my_error("unknown tag2 %x\r\n", bOperater);
                    MissCode_PrintHex();
                    goto error;
                }
                break;

            default:
                if ( 0 == MissCode_OperaterProc(acTempBuf[0]) )
                {
                    my_error("proc operater %x fail\r\n", acTempBuf[0]);
                    goto error;
                }
                break;
        }
    }

    //写回bin文件
    for ( i = 0; i < m_uiMissCodeCount; i++ )
    {
        memset(m_acGlobalBuffer, 0, m_iGlobaBufLength);
        sprintf(m_acGlobalBuffer, "%u\t%s\r\n\t", i, m_astMissCode[i].acExpressionTxt);

        for ( j = 0; j < m_astMissCode[i].uiBinLen; j++ )
        {
            sprintf(&m_acGlobalBuffer[strlen(m_acGlobalBuffer)], "%02x ", m_astMissCode[i].acExpressionBin[j]);
        }

        strcpy(&m_acGlobalBuffer[strlen(m_acGlobalBuffer)], "\r\n");

        fwrite(m_acGlobalBuffer, 1, strlen(m_acGlobalBuffer), pfOutputHandle);
    }

    goto out;

error:
    MissCode_PrintHex();

    if ( 0 < m_uiMissCodeCount )
    {
        my_error("last expression: %s\r\n", m_astMissCode[m_uiMissCodeCount - 1].acExpressionTxt);
        m_uiMissCodeCount--;
    }
    if ( 0 < m_uiMissCodeCount )
    {
        my_error("last expression: %s\r\n", m_astMissCode[m_uiMissCodeCount - 1].acExpressionTxt);
        m_uiMissCodeCount--;
    }
    if ( 0 < m_uiMissCodeCount )
    {
        my_error("last expression: %s\r\n", m_astMissCode[m_uiMissCodeCount - 1].acExpressionTxt);
        m_uiMissCodeCount--;
    }

    result = 0;

out:
    Stack_Release(m_pvStack);

    if ( NULL != pfBinHandle )
    {
        fclose(pfBinHandle);
        pfBinHandle = NULL;
    }
    if ( NULL != pfOutputHandle )
    {
        fclose(pfOutputHandle);
        pfOutputHandle = NULL;
    }

    if ( 0 == result )
    {
        my_error("parsing %s.bin failed!\r\n", FILE_PREFIX);
    }
    else
    {
        my_printf("parsing %s.bin success with %d lines!\r\n", FILE_PREFIX, m_uiMissCodeCount);
    }

    return result;
}

char *MissCode_GetExpression(unsigned int id)
{
    unsigned int i;
    unsigned int uiOffset = 0;

    if ( 0xFFFFFFFF <= id )
    {
        return NULL;
    }

    for ( i = 0; i < m_uiMissCodeCount; i++ )
    {
        if ( uiOffset == id)
        {
            return m_astMissCode[i].acExpressionTxt;
        }

        uiOffset += m_astMissCode[i].uiBinLen;
    }

    return NULL;
}

