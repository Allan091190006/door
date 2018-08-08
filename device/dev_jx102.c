int  JX102R_Open_Channel(char achannel, bool aManualClose, char *Ip, int port)
{
	int ret;
	gSendToPort = port;
	TSendData SendData;
	memset((void *)&SendData, 0, sizeof(TSendData));
	SendData.DevType = 0x80;
	SendData.DevAdr = 0xFFFF;
	SendData.SorAdr = 0x0000;
	SendData.Cmd = REMOTE_OPEN;
	SendData.AckType = 0xAA;
	SendData.Sequence = 0x0001;
	SendData.DataBlock[0] = achannel;
	if (aManualClose)
		SendData.DataBlock[1] = 0x1;
	else
		SendData.DataBlock[1] = 0x0;

	SendData.Block_len = 2;
	ret = WriteData(&SendData);
	if (ret != 0)
	{
		return ret;
	}
	else
	{
		TReceiveData ReceiveData;
		ret = ReadData(&ReceiveData);
		if (ret != 0)
			return ret;
		else
		{
			if (ReceiveData.AckType == 0)
				return 0;
			else
			{
				printf("Receive data ack type, {}\n", ReceiveData.AckType);
				return ReceiveData.AckType;
			}
		}

	}
}