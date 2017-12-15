/*
 * eth.c
 *
 *  Created on: Nov 2, 2017
 *      Author: chentuofei
 */

// Globals for writing in sys tick routine
#include "usr_eth.h"

float pset_1 = 0.0;
float pset_2 = 0.0;

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    /* do not read the packet if we are not in ESTABLISHED state */
    if (!p)
    {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    /* indicate that the packet has been received */
    tcp_recved(tpcb, p->len);

    /* echo back the payload */
    /* in this case, we assume that the payload is < TCP_SND_BUF */
    if (tcp_sndbuf(tpcb) > p->len)
    {
        char buf[20];
        err = eth_send(p->payload, p->len); // echo back
        pset_1 = atof(p->payload);
        pset_2 = atof(p->payload + 8);

        UARTprintf("Got: %s len: %d echo err %d\n\r",p->payload,p->len,err);
    }
    else
        UARTprintf("no space in tcp_sndbuf\n\r");

    /* free the received pbuf */
    pbuf_free(p);

    return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    static int connection = 1; // initialize only once

    char t_str[20];
    int msg_size = usprintf(t_str, "Handshake...\n\r"); // debug info

    /* just use an integer number indicating the connection id as the
     callback argument */
    tcp_arg(newpcb, (void*) (uint32_t*) connection);

    /* set the receive callback for this connection */
    tcp_recv(newpcb, recv_callback);

    /* increment for subsequent accepted connections */
    connection++;
    pcb_glob = newpcb;

    eth_send(t_str, msg_size); // handshake

    return ERR_OK;
}

void eth_init()
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];

    //
    // Configure SysTick for a periodic interrupt for service LWIP.
    //

    MAP_SysTickPeriodSet(g_ui32SysClock / SYSTICKHZ);
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();

    //
    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.  The MAC address will be stored in the non-volatile
    // USER0 and USER1 registers.
    //
    MAP_FlashUserGet(&ui32User0, &ui32User1);
    if ((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address has
        // not been programmed into the device.  Exit the program.
        // Let the user know there is no MAC address
        //
        UARTprintf("No MAC programmed!\n");
        while (1)
        {
        }
    }
    //
    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    //
    pui8MACArray[0] = ((ui32User0 >> 0) & 0xff);
    pui8MACArray[1] = ((ui32User0 >> 8) & 0xff);
    pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACArray[3] = ((ui32User1 >> 0) & 0xff);
    pui8MACArray[4] = ((ui32User1 >> 8) & 0xff);
    pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

    uint32_t gateIP = 3232236033;
    uint32_t serverIP = gateIP + 4; //.5
    uint16_t serverPort = 10000;

    // init network interface
    lwIPInit(g_ui32SysClock, pui8MACArray, gateIP + 2, 0xffffff00, gateIP,
             IPADDR_USE_STATIC);
    // init pcb callbacks
    start_application(serverIP, serverPort); // start app by connect to laptop server

    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    //
    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

}

int start_application(uint32_t serverIP, uint16_t serverPort)
{
    struct tcp_pcb* pcb;
    struct ip_addr serverIpStruct;
    err_t err;
    unsigned localPort = 23;  // why 7

    uint32_t frame_mem = 1500 * 20;
    send_cnt = frame_mem;
    seq_num = 0;

    /* create new TCP PCB structure */
    pcb = tcp_new();
    if (!pcb)
    {
        UARTprintf("Error creating PCB. Out of Memory\n\r");
        return -1;
    }
    tcp_setprio(pcb, 1);
    /* bind to specified @port */
    err = tcp_bind(pcb, IP_ADDR_ANY, localPort);
    if (err != ERR_OK)
    {
        UARTprintf("Unable to bind to port %d: err = %d\n\r", localPort, err);
        return -2;
    }

    tcp_arg(pcb, NULL); // accept callback argument
    /*    use if setup as server, listen for connections */
//    pcb = tcp_listen(pcb);
//    if (!pcb) {
//        UARTprintf("Out of memory while tcp_listen\n\r");
//        return -3;
//    }
//    tcp_accept(pcb, accept_callback);

    /*    use if set up as client (accept_callback is overloaded) */
    serverIpStruct.addr = htonl(serverIP);
    err = tcp_connect(pcb, &serverIpStruct, serverPort, accept_callback);

    if (err != ERR_OK)
    {
        UARTprintf("Unable to Connect to Server err = %d\n\r", err);
        return -4;
    }
    else
    {
        char pcBuf[16];
        usprintf(pcBuf, "%d.%d.%d.%d", (serverIP >> 24) & 0xff,
                 (serverIP >> 16) & 0xff, (serverIP >> 8) & 0xff,
                 serverIP & 0xff);

        UARTprintf("ETH: Connected %s, port %d\n\r", pcBuf,
                   serverPort);
    }
//    UARTprintf("TCP echo server started @ port %d\n\r", port);

    return 0;
}

err_t eth_send(char* msg, int size)
{
    if (pcb_glob != NULL)
    {
        int32_t availbuf = tcp_sndbuf(pcb_glob);
        UARTprintf("Available buf %d\n", availbuf);

        if (availbuf > 1460 * 8)
            availbuf = 1460 * 8;

        strcpy(msg_str, msg); // so that when msg is reclaimed, msg_str is gloabl

        err_write = tcp_write(pcb_glob, msg_str, size, 0);

        if (err_write)
        {
            UARTprintf("tcp_write error in sys tick handler %d\n", err_write);
            if (err_write == ERR_CONN) // not connected flag
            {
                UARTprintf("disconnected \n");
                pcb_glob = NULL;
            }
        }

        err_write = tcp_output(pcb_glob);
        if (err_write)
        {
            UARTprintf("tcp_output error in sys tick handler %c\n", err_write);
            if (err_write == ERR_CONN) // not connected flag
            {
                UARTprintf("disconnected \n");
                pcb_glob = NULL;
            }
        }

    }
    return err_write;
}

void SysTickIntHandler(void)
{
    // Call the lwIP timer handler. (service all lwip related callback)
    //
    lwIPTimer(SYSTICKMS);
}
