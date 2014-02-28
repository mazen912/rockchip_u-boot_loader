/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    CHAP_9.C
Author:         XUESHAN LIN
Created:        1st Dec 2008
Modified:
Revision:       1.00
********************************************************************************
********************************************************************************/
#define     IN_CHAP9
#include    "../../armlinux/config.h"
#include 	"USB20.h"		//USB������ͷ�ļ�

/**************************************************************************
��������״̬
***************************************************************************/
void set_address(void)
{
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG_BASE_ADDR;

    OtgReg->Device.dcfg=(OtgReg->Device.dcfg & (~0x07f0)) | (ControlData.DeviceRequest.wValue << 4);  //reset device addr
    ep0in_ack();
}


/**************************************************************************
��ȡ������
***************************************************************************/
void get_descriptor(void)
{
    uint16 length=0;
    USB_DEVICE_DESCRIPTOR * pDeviceDescr;
    uint16 bDescriptor = (ControlData.DeviceRequest.wValue >> 8) & 0xff;
    uint8 *p;

    switch (bDescriptor)
    {
        case USB_DEVICE_DESCRIPTOR_TYPE: 				//��ȡ�豸������
            length=sizeof(USB_DEVICE_DESCRIPTOR);
            if (length >= ControlData.DeviceRequest.wLength)
                length = ControlData.DeviceRequest.wLength;

            ftl_memcpy(Ep0Buf, (uint8*)&HSDeviceDescr, length);
#if(RK_MODIFY_USB_VIDPID_EN)
            pDeviceDescr = (USB_DEVICE_DESCRIPTOR *)Ep0Buf;
            ModifyUsbVidPid(pDeviceDescr);
#endif			
            if (BulkEpSize==FS_BULK_TX_SIZE)
            {
                USB_DEVICE_DESCRIPTOR *ptr;
                ptr=(USB_DEVICE_DESCRIPTOR*)Ep0Buf;
                ptr->bcdUSB=0x0111;//loader 0x0111 0x0201
            }
            break;
        case USB_CONFIGURATION_DESCRIPTOR_TYPE: 		//��ȡ����������
            length=CONFIG_DESCRIPTOR_LENGTH;
            if (length >= ControlData.DeviceRequest.wLength)
                length = ControlData.DeviceRequest.wLength;

            ftl_memcpy(Ep0Buf, (uint8*)&HSConfigDescr, length);
            if (BulkEpSize==FS_BULK_TX_SIZE)
            {
                USB_CONFIGS_DESCRIPTOR *ptr;
                ptr=(USB_CONFIGS_DESCRIPTOR*)Ep0Buf;
                ptr->BulkIn.wMaxPacketSize=FS_BULK_TX_SIZE;
                //ptr->BulkOut.wMaxPacketSize=FS_BULK_TX_SIZE;
                p = (uint8*)&(ptr->BulkOut.wMaxPacketSize);  //�������half-word���ݣ���ַȴû��half-word���룬����CPU��abort������˸���byte������
                *p++ = FS_BULK_TX_SIZE;
                *p = 0;
            }
            break;
        #if 0
        case USB_STRING_DESCRIPTOR_TYPE: 				//��ȡ�ַ���������
            switch (ControlData.DeviceRequest.wValue & 0x00ff)
            {
                case 0xee:  //Micorsoft OS Description
                    length=ControlData.DeviceRequest.wLength;
            		if (length > sizeof(MicrosoftOSDesc))
            			length=sizeof(MicrosoftOSDesc);
                    ftl_memcpy(Ep0Buf, (uint8*)&MicrosoftOSDesc, length);
            		ControlData.pData=(uint8*)&Ep0Buf[0];
            		ControlData.wLength=length;
            		if (length > Ep0PktSize)
            			length=Ep0PktSize;
            		WriteEndpoint0(length, ControlData.pData);
                    break;
            	default:
                    length=ControlData.DeviceRequest.wLength;
                    if (length > StringManufacture[0])
                        length=StringManufacture[0];
                    ftl_memcpy(Ep0Buf, (uint8*)&StringManufacture, length);
                    break;
            }
            break;
        #endif
        default:
            stall_ep0();
            break;
    }
    ControlData.pData=(uint8*)&Ep0Buf[0];
    ControlData.wLength=length;
    if (length > Ep0PktSize)
        length=Ep0PktSize;

    WriteEndpoint0(length, Ep0Buf);
    ControlData.pData += length;
    ControlData.wLength -= length;
    if (ControlData.wLength == 0)
        ControlStage=STAGE_STATUS;
    else
        ControlStage=STAGE_DATA;
}

/**************************************************************************
��������״̬
***************************************************************************/
void set_configuration(void)
{
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG_BASE_ADDR;
    OtgReg->Device.InEp[BULK_IN_EP].DiEpCtl =(1<<28)|(BULK_IN_EP<<22)|(2<<18)|(1<<15)|0x200;//|(1<<27)
    ep0in_ack();
}

/**************************************************************************
�˵�0 INӦ��
***************************************************************************/
void ep0in_ack(void)
{
    WriteEndpoint0(0, NULL);
}


/***************************************************************************
����stallӦ��
***************************************************************************/
void stall_ep0(void)
{
    pUSB_OTG_REG OtgReg=(pUSB_OTG_REG)USB_OTG_BASE_ADDR;

    OtgReg->Device.OutEp[0].DoEpCtl |= 1<<21;  //send OUT0 stall handshack
    OtgReg->Device.InEp[0].DiEpCtl |= 1<<21;   //send IN0 stall handshack
}

