#include "sendmail.hpp"
#include "json_parser.hpp"

#include <libesmtp.h>

#define DEFAULT_SUBJECT "PEF OEM MAIL"
#define DEFAULT_FROM "IPMI_PEF"
#define BUFF_SIZE 1024
#define BUFF_ERR_SIZE 128

using namespace phoenix::dbus::monitor;

int sendmail(char *context) {
    smtp_session_t session;
    smtp_message_t message;
    struct sigaction sa;
    const smtp_status_t *status;
    char buf[BUFF_ERR_SIZE];
    char mail[BUFF_SIZE];

	memset(mail, 0, BUFF_SIZE);

	/* RFC822 message */
	sprintf (mail,
		"MIME-Version: 1.0\r\n"
		"Content-Type: text/plain;\r\n"
		"Content-Transfer-Encoding: 8bit\r\n"
		"\r\n"
		"%s\r\n"
		"\r\n"
		, context);

    /* This program sends only one message at a time. Create an SMTP
       session and add a message to it. */
    if( (session = smtp_create_session ()) == NULL){
        fprintf (stderr, "smtp_create_session problem %s\n",
                smtp_strerror (smtp_errno (), buf, sizeof buf));
        return 1;
    }
    if((message = smtp_add_message (session)) == NULL){
        fprintf (stderr, "smtp_add_message problem %s\n",
                smtp_strerror (smtp_errno (), buf, sizeof buf));
        return 1;
    }

    smtp_set_hostname(session, "pef");
    smtp_set_message_str(message, mail);

    /* NB. libESMTP sets timeouts as it progresses through the protocol.
       In addition the remote server might close its socket on a timeout.
       Consequently libESMTP may sometimes try to write to a socket with
       no reader. Ignore SIGPIPE, then the program doesn't get killed
       if/when this happens. */
    sa.sa_handler = SIG_IGN;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction (SIGPIPE, &sa, NULL); 
    /* Set the host running the SMTP server. LibESMTP has a default port
       number of 587, however this is not widely deployed so the port
       is specified as 25 along with the default MTA host. */
    smtp_set_server (session, (mail_table.smtpIP).c_str());
    /* RFC 2822 doesn't require recipient headers but a To: header would
     * be nice to have if not present. */
    smtp_set_header (message, "To", NULL, NULL);
    smtp_set_header (message, "Subject", DEFAULT_SUBJECT);
    smtp_set_header_option (message, "Subject", Hdr_OVERRIDE, 1);
    smtp_set_header (message, "From", DEFAULT_FROM, DEFAULT_FROM);
    fprintf(stderr,"%s\n","smtp_set_server.");
    smtp_add_recipient (message, (mail_table.recipient).c_str());
    /* Initiate a connection to the SMTP server and transfer the
       message. */
    if (!smtp_start_session (session)){
        fprintf (stderr, "SMTP server problem %s\n",
                smtp_strerror (smtp_errno (), buf, sizeof buf));
    }
    else{
        /* Report on the success or otherwise of the mail transfer.
        */
        status = smtp_message_transfer_status (message);
        printf ("%d %s", status->code,
                (status->text != NULL) ? status->text : "\n");
    }
    /* Free resources consumed by the program.
    */
    smtp_destroy_session (session);
    return 0;
}

void email_context(uint8_t id, uint8_t sensorType, uint8_t sensorNum
    , std::vector<uint8_t> eventData, char* context)
{
	sprintf(context, "RecordID       = 0x%04x\n"
                     "Sensor Type    = 0x%02x\n"
                     "Sensor Num     = 0x%02x\n"
                     "Event Data1    = 0x%02x\n"
                     "Event Data2    = 0x%02x\n"
                     "Event Data3    = 0x%02x\n"
        , id, sensorType, sensorNum, eventData[0], eventData[3], eventData[2]);
}

int email_action(uint8_t id, uint8_t sensorType, uint8_t sensorNum
    , std::vector<uint8_t> eventData)
{
    char *context;
	context = (char *)malloc(sizeof(char) * BUFF_SIZE);
	memset(context, 0, BUFF_SIZE);

    email_context(id, sensorType, sensorNum, eventData, context);
    sendmail(context);

    free(context);

    return 0;
}
