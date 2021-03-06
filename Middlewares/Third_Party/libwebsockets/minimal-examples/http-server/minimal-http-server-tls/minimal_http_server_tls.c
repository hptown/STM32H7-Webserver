/*
 * lws-minimal-http-server-tls
 *
 * Copyright (C) 2018 Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This demonstrates the most minimal http server you can make with lws,
 * with three extra lines giving it tls (ssl) capabilities, which in
 * turn allow operation with HTTP/2 if lws was configured for it.
 *
 * To keep it simple, it serves stuff from the subdirectory 
 * "./mount-origin" of the directory it was started in.
 *
 * You can change that by changing mount.origin below.
 */

#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include "cmsis_os.h"

void minimal_http_server_tls (void const *argument);

static const struct lws_http_mount mount = {
	/* .mount_next */		NULL,		/* linked-list "next" */
	/* .mountpoint */		"/",		/* mountpoint URL */
	/* .origin */			"./mount-origin", /* serve from dir */
	/* .def */			"index.html",	/* default filename */
	/* .protocol */			NULL,
	/* .cgienv */			NULL,
	/* .extra_mimetypes */		NULL,
	/* .interpret */		NULL,
	/* .cgi_timeout */		0,
	/* .cache_max_age */		0,
	/* .auth_mask */		0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file */	NULL,
};


void minimal_http_server_tls (void const *argument)
{
	struct lws_context_creation_info info;
	struct lws_context *context;
	//const char *p;
	int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
			/* for LLL_ verbosity above NOTICE to be built into lws,
			 * lws must have been configured and built with
			 * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
			 | LLL_INFO  /* | LLL_PARSER */ /* | LLL_HEADER */
			/* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
			/* | LLL_DEBUG */;

	//if ((p = lws_cmdline_option(argc, argv, "-d")))
		//logs = atoi(p);

	lws_set_log_level(logs, NULL);
	lwsl_user("LWS minimal http server TLS | visit https://localhost:7681\n");

	//signal(SIGINT, sigint_handler);

	memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = 443;
	info.mounts = &mount;
	info.error_document_404 = "/404.html";
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT |
		LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
	//info.ssl_cert_filepath = "localhost-100y.cert";
	//info.ssl_private_key_filepath = "localhost-100y.key";
	info.ssl_cert_filepath = "stm32-h753.cert";
	info.ssl_private_key_filepath = "stm32-h753.key";

	//if (lws_cmdline_option(argc, argv, "-h"))
		//info.options |= LWS_SERVER_OPTION_VHOST_UPG_STRICT_HOST_CHECK;

	context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		goto exit;
	}

	while (n >= 0)
		n = lws_service(context, 100);
exit:
	lws_context_destroy(context);

	while(1)
	{
		osDelay(100);
	}
}
