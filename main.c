#include <lwip/tcp.h>
#include <stdio.h>
#include <string.h>
#include <netif/etharp.h>
static err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t server_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
static err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t server_poll(void *arg, struct tcp_pcb *pcb);
void my_server_init(void);


struct netif netif;
int main(int argc, char** argv){
	printf("ddd\n");
	mem_init();
	lwip_init();
	my_server_init();
	while(1){
	}
	return 0;
}

void my_server_init(void)
{
	printf("init \n");
	struct tcp_pcb *pcb;
	pcb = tcp_new();
	err_t err = tcp_bind(pcb, IP_ADDR_ANY, 8000); //server port for incoming connection
	if(err != ERR_OK){
		printf("bind err\n");
		return;
	}
	pcb = tcp_listen(pcb);
	tcp_arg(pcb,pcb);
	tcp_accept(pcb, server_accept);
	printf("init finished\n");
}

static void server_close(struct tcp_pcb *pcb)
{
	tcp_arg(pcb, NULL);
	tcp_sent(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_close(pcb);
        // printf("\nserver_close(): Closing...\n");
}

static err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	printf("server accept");
	struct tcp_pcb_listen* server = arg;
	tcp_accepted(server);
        tcp_recv(pcb, server_recv);
        tcp_poll(pcb, server_poll, 4);
	tcp_sent(pcb, server_sent);
        return ERR_OK;
}

static err_t server_poll(void *arg, struct tcp_pcb *pcb)
{
	static int counter = 1;

	printf("\nserver_poll(): Call number %d\n", counter++);
	return ERR_OK;
}

static err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err){
        char *string;
	size_t length;
        if (err == ERR_OK && p != NULL) {
                tcp_recved(pcb, p->tot_len);
                string = p->payload;
		length = strlen(string);
		tcp_write(pcb, string, length,0);
		pbuf_free(p);
        }else {
                if (err != ERR_OK)
        // 		printf("Connection is not on ERR_OK state : %d \n", err);
                if (p == NULL)
          //              printf("Pbuf pointer p is a NULL pointer : \n ");
            //    printf("Closing server-side connection...");
                pbuf_free(p);
                server_close(pcb);
        }
        return ERR_OK;
}

static err_t server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	return ERR_OK;
}
