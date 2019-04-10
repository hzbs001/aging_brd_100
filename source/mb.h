#define MB_PDU_SIZE_MAX 253

/* |slaveAddr|funcCode| data | CRC| */
/* |    1    |   1    | 0-252|  2 | */
#define MB_FRAME_SIZE_MAX 255

#define REG_HOLDING_NREGS 120
#define REG_INPUT_NREGS 2
typedef enum MB_FLAGS
{
	FrameFlag,
	TX_EN
} MB_FLAGS;

typedef enum MB_CNT
{
	RX_CNT,
	TX_CNT
} MB_CNT;

typedef enum RD_WR
{
	READ,
	WRITE
} RD_WR;

void mb_Init(void);
void mb_SendData(uint8_t *buff,uint8_t len);
void mb_Service(void);
void mbRcvComplete(void);
void mbRcvProcess(void);
void updateReg2(void);
//void mb_RegMap(uint16_t **mb_Inputreg,uint16_t **mb_Outputreg,uint16_t **mb_Holdreg);
//void mb_CtrMap(uint8_t *p_rcv_buf,uint16_t *p_rcv_cnt,uint8_t *p_frameFlag,uint8_t *p_tx_en);
void setMBAddr(uint8_t addr);

void mbRegHolding(uint8_t *p,uint16_t startRegAddr,uint16_t RegNum,RD_WR mode);

void Modbus_01_Solve(void);
void Modbus_02_Solve(void);
void Modbus_03_Solve(void);
void Modbus_05_Solve(void);
void Modbus_06_Solve(void);
void Modbus_15_Solve(void);
void Modbus_16_Solve(void);
