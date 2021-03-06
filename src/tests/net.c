#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <darwintest.h>

T_DECL(link_ntoa_basic, "link_ntoa converts to proper string")
{
//	u_char	sdl_len;	/* Total length of sockaddr */
//	u_char	sdl_family;	/* AF_LINK */
//	u_short	sdl_index;	/* if != 0, system given index for interface */
//	u_char	sdl_type;	/* interface type */
//	u_char	sdl_nlen;	/* interface name length, no trailing 0 reqd. */
//	u_char	sdl_alen;	/* link level address length */
//	u_char	sdl_slen;	/* link layer selector length */
//	char	sdl_data[12];	/* minimum work area, can be larger;
//				   contains both if name and ll address */

	struct sockaddr_dl sad;
	bzero(&sad, sizeof(sad));
	sad.sdl_nlen = 3;
	sad.sdl_len = 0;
	sad.sdl_data[0] = 'l';
	sad.sdl_data[1] = 'e';
	sad.sdl_data[2] = '0';
	sad.sdl_data[3] = 0x01;
	sad.sdl_data[4] = 0x80;
	sad.sdl_data[5] = 0xc2;
	sad.sdl_data[6] = 0x00;
	sad.sdl_data[7] = 0x00;
	sad.sdl_data[8] = 0x02;
	sad.sdl_data[9] = 0xaa;
	sad.sdl_data[10] = 0xbb;
	sad.sdl_data[11] = 0xcc;
	sad.sdl_alen = 6;

	char *foo = link_ntoa(&sad);

	T_EXPECT_EQ_STR("le0:1.80.c2.0.0.2", foo, NULL);
}

T_DECL(link_ntoa_overflow, "link_ntoa try to overflow")
{
	char sockraw[64];
	struct sockaddr_dl *sad;
	sad = (struct sockaddr_dl *)&sockraw;
	bzero(sad, sizeof(*sad));
	sad->sdl_nlen = 3;
	sad->sdl_len = 0;
	sad->sdl_data[0] = 'l';
	sad->sdl_data[1] = 'e';
	sad->sdl_data[2] = '0';
	sad->sdl_data[3] = 0x11;
	sad->sdl_data[4] = 0x80;
	sad->sdl_data[5] = 0xc2;
	sad->sdl_data[6] = 0x11;
	sad->sdl_data[7] = 0x11;
	sad->sdl_data[8] = 0xa2;
	sad->sdl_data[9] = 0xaa;
	sad->sdl_data[10] = 0xbb;
	sad->sdl_data[11] = 0xcc;
	sockraw[20] = 0xdd;
	sockraw[21] = 0xee;
	sockraw[22] = 0xff;
	sockraw[23] = 0x1a;
	sockraw[24] = 0x1b;
	sockraw[25] = 0x1c;
	sockraw[26] = 0x1d;
	sockraw[27] = 0x1e;
	sockraw[28] = 0x1f;
	sockraw[29] = 0x2a;
	sockraw[30] = 0x2b;
	sockraw[31] = 0x2c;

	/* set the length to something that will fit in the buffer */
	sad->sdl_alen = 20;

	char *foo = link_ntoa(sad);

	char over = foo[64];
	char over2 = foo[65];

	/* this string should be 66 bytes long and exceed the buffer */
	sad->sdl_alen = 21;

	foo = link_ntoa(sad);

	T_EXPECT_EQ_STR("", foo, NULL);

	T_EXPECT_EQ(over, foo[64], "did not overflow");
	T_EXPECT_EQ(over2, foo[65], "did not overflow either");
}

T_DECL(inet_ntop, "inet_ntop")
{
	char *addresses4[] = { "1.2.3.4", "10.0.0.1", "2.2.2.2" };
	char *addresses6[] = {
		"2001:db8:85a3::8a2e:370:7334",
		"::1", "::"
	};
	for (int i = 0; i < sizeof(addresses4)/sizeof(addresses4[0]); i++){
		struct in_addr addr4;
		char buf[64];
		T_EXPECT_EQ(inet_pton(AF_INET, addresses4[i], &addr4), 1, "inet_pton(AF_INET, %s)", addresses4[i]);
		char *str = inet_ntop(AF_INET, &addr4, buf, sizeof(buf));
		T_EXPECT_NOTNULL(str, "inet_ntop(AF_INET) of %s", addresses4[i]);
		T_EXPECT_EQ_STR(str, addresses4[i], "round-trip of %s", addresses4[i]);
	}
	for (int i = 0; i < sizeof(addresses6)/sizeof(addresses6[0]); i++){
		struct in6_addr addr6;
		char buf[64];
		T_EXPECT_EQ(inet_pton(AF_INET6, addresses6[i], &addr6), 1, "inet_pton(AF_INET6, %s)", addresses6[i]);
		char *str = inet_ntop(AF_INET6, &addr6, buf, sizeof(buf));
		T_EXPECT_NOTNULL(str, "inet_ntop(AF_INET) of %s", addresses6[i]);
		T_EXPECT_EQ_STR(str, addresses6[i], "round-trip of %s", addresses6[i]);
	}
}
