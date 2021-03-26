/*
** Swirl @(#)swirl.c	1.6
*/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <termio.h>
#ifdef	SYS_STROPTS_H
#include <sys/stropts.h>
#endif

#define	IFLAG	0
#define OFLAG	1
#define	CFLAG	2
#define	LFLAG	3

#define FALSE	0
#define TRUE	1

struct termio termio;

#ifdef IP22
tcflag_t *FlagPtrs[4] = { &termio.c_iflag, &termio.c_oflag, &termio.c_cflag, &termio.c_lflag };
#else
unsigned short *FlagPtrs[4] = { &termio.c_iflag, &termio.c_oflag, &termio.c_cflag, &termio.c_lflag };
#endif

struct stty_opt
{
    char *name;			/* name of option: CLOCAL, TAB3, BS1 etc 			 */
    int  element;		/* which termio flag to modify (CFLAG, IFLAG, OFLAG, LFLAG)	 */
    int  clrbits;		/* which bits are to be ANDed out if the option is selected	 */
    int	 setbits;		/* which bits are to be ORed in if the option is selected	 */
    int  yes;			/* TRUE if this option was selected				 */
} stty_opts[] =
{
    { "ignbrk",		IFLAG,	~IGNBRK,	IGNBRK	},
    { "-ignbrk",	IFLAG,	~IGNBRK,	0	},
    { "brkint",		IFLAG,	~BRKINT,	BRKINT	},
    { "-brkint",	IFLAG,	~BRKINT,	0	},
    { "ignpar",		IFLAG,	~IGNPAR,	IGNPAR	},
    { "-ignpar",	IFLAG,	~IGNPAR,	0	},
    { "parmrk",		IFLAG,	~PARMRK,	PARMRK	},
    { "-parmrk",	IFLAG,	~PARMRK,	0	},
    { "inpck",		IFLAG,	~INPCK,		INPCK	},
    { "-inpck",		IFLAG,	~INPCK,		0	},
    { "istrip",		IFLAG,	~ISTRIP,	ISTRIP	},
    { "-istrip",	IFLAG,	~ISTRIP,	0	},
    { "inlcr",		IFLAG,	~INLCR,		INLCR	},
    { "-inlcr",		IFLAG,	~INLCR,		0	},
    { "igncr",		IFLAG,	~IGNCR,		IGNCR	},
    { "-igncr",		IFLAG,	~IGNCR,		0	},
    { "icrnl",		IFLAG,	~ICRNL,		ICRNL	},
    { "-icrnl",		IFLAG,	~ICRNL,		0	},
    { "iuclc",		IFLAG,	~IUCLC,		IUCLC	},
    { "-iuclc",		IFLAG,	~IUCLC,		0	},
    { "ixon",		IFLAG,	~IXON,		IXON	},
    { "-ixon",		IFLAG,	~IXON,		0	},
    { "ixany",		IFLAG,	~IXANY,		IXANY	},
    { "-ixany",		IFLAG,	~IXANY,		0	},
    { "ixoff",		IFLAG,	~IXOFF,		IXOFF	},
    { "-ixoff",		IFLAG,	~IXOFF,		0	},
    { "imaxbel",	IFLAG,	~IMAXBEL,	IMAXBEL	},
    { "-imaxbel",	IFLAG,	~IMAXBEL,	0	},
#ifdef DOSMODE
    { "dosmode",	IFLAG,	~DOSMODE,	DOSMODE	},
    { "-dosmode",	IFLAG,	~DOSMODE,	0	},
#endif

    { "opost",		OFLAG,	~OPOST,		OPOST	},
    { "-opost",		OFLAG,	~OPOST,		0	},
    { "olcuc",		OFLAG,	~OLCUC,		OLCUC	},
    { "-olcuc",		OFLAG,	~OLCUC,		0	},
    { "onlcr",		OFLAG,	~ONLCR,		ONLCR	},
    { "-onlcr",		OFLAG,	~ONLCR,		0	},
    { "ocrnl",		OFLAG,	~OCRNL,		OCRNL	},
    { "-ocrnl",		OFLAG,	~OCRNL,		0	},
    { "onocr",		OFLAG,	~ONOCR,		ONOCR	},
    { "-onocr",		OFLAG,	~ONOCR,		0	},
    { "onlret",		OFLAG,	~ONLRET,	ONLRET	},
    { "-onlret",	OFLAG,	~ONLRET,	0	},
    { "ofill",		OFLAG,	~OFILL,		OFILL	},
    { "-ofill",		OFLAG,	~OFILL,		0	},
    { "ofdel",		OFLAG,	~OFDEL,		OFDEL	},
    { "-ofdel",		OFLAG,	~OFDEL,		0	},

    { "nl0",		OFLAG,	~NLDLY,		NL0	},
    { "nl1",		OFLAG,	~NLDLY,		NL1	},
    { "-nldly",		OFLAG,	~NLDLY,		0	},
    { "cr0",		OFLAG,	~CRDLY,		CR0	},
    { "cr1",		OFLAG,	~CRDLY,		CR1	},
    { "cr2",		OFLAG,	~CRDLY,		CR2	},
    { "cr3",		OFLAG,	~CRDLY,		CR3	},
    { "-crdly",		OFLAG,	~CRDLY,		0	},
    { "tab0",		OFLAG,	~TABDLY,	TAB0	},
    { "tab1",		OFLAG,	~TABDLY,	TAB1	},
    { "tab2",		OFLAG,	~TABDLY,	TAB2	},
    { "tab3",		OFLAG,	~TABDLY,	TAB3	},
    { "xtabs",		OFLAG,	~TABDLY,	XTABS	},
    { "-tabdly",	OFLAG,	~TABDLY,	0	},
    { "bs0",		OFLAG,	~BSDLY,		BS0	},
    { "bs1",		OFLAG,	~BSDLY,		BS1	},
    { "-bsdly",		OFLAG,	~BSDLY,		0	},
    { "vt0",		OFLAG,	~VTDLY,		VT0	},
    { "vt1",		OFLAG,	~VTDLY,		VT1	},
    { "-vtdly",		OFLAG,	~VTDLY,		0	},
    { "ff0",		OFLAG,	~FFDLY,		FF0	},
    { "ff1",		OFLAG,	~FFDLY,		FF1	},
    { "-ffdly",		OFLAG,	~FFDLY,		0	},

    { "cs5",		CFLAG,	~CSIZE,		CS5	},
    { "cs6",		CFLAG,	~CSIZE,		CS6	},
    { "cs7",		CFLAG,	~CSIZE,		CS7	},
    { "cs8",		CFLAG,	~CSIZE,		CS8	},
    { "cstopb",		CFLAG,	~CSTOPB,	CSTOPB	},
    { "-cstopb",	CFLAG,	~CSTOPB,	0	},
    { "cread",		CFLAG,	~CREAD,		CREAD	},
    { "-cread",		CFLAG,	~CREAD,		0	},
    { "parenb",		CFLAG,	~PARENB,	PARENB	},
    { "-parenb",	CFLAG,	~PARENB,	0	},
    { "parodd",		CFLAG,	~PARODD,	PARODD	},
    { "-parodd",	CFLAG,	~PARODD,	0	},
    { "hupcl",		CFLAG,	~HUPCL,		HUPCL	},
    { "-hupcl",		CFLAG,	~HUPCL,		0	},
    { "clocal",		CFLAG,	~CLOCAL,	CLOCAL	},
    { "-clocal",	CFLAG,	~CLOCAL,	0	},

    { "isig",		LFLAG,	~ISIG,		ISIG	},
    { "-isig",		LFLAG,	~ISIG,		0	},
    { "icanon",		LFLAG,	~ICANON,	ICANON	},
    { "-icanon",	LFLAG,	~ICANON,	0	},
    { "xcase",		LFLAG,	~XCASE,		XCASE	},
    { "-xcase",		LFLAG,	~XCASE,		0	},
    { "echo",		LFLAG,	~ECHO,		ECHO	},
    { "-echo",		LFLAG,	~ECHO,		0	},
    { "echoe",		LFLAG,	~ECHOE,		ECHOE	},
    { "-echoe",		LFLAG,	~ECHOE,		0	},
    { "echok",		LFLAG,	~ECHOK,		ECHOK	},
    { "-echok",		LFLAG,	~ECHOK,		0	},
    { "echonl",		LFLAG,	~ECHONL,	ECHONL	},
    { "-echonl",	LFLAG,	~ECHONL,	0	},
    { "noflsh",		LFLAG,	~NOFLSH,	NOFLSH	},
    { "-noflsh",	LFLAG,	~NOFLSH,	0	},
    { "tostop",		LFLAG,	~TOSTOP,	TOSTOP	},
    { "-tostop",	LFLAG,	~TOSTOP,	0	},
    { "echoctl",	LFLAG,	~ECHOCTL,	ECHOCTL	},
    { "-echoctl",	LFLAG,	~ECHOCTL,	0	},
    { "echoprt",	LFLAG,	~ECHOPRT,	ECHOPRT	},
    { "-echoprt",	LFLAG,	~ECHOPRT,	0	},
    { "echoke",		LFLAG,	~ECHOKE,	ECHOKE	},
    { "-echoke",	LFLAG,	~ECHOKE,	0	},
    { "flusho",		LFLAG,	~FLUSHO,	FLUSHO	},
    { "-flusho",	LFLAG,	~FLUSHO,	0	},
    { "pendin",		LFLAG,	~PENDIN,	PENDIN	},
    { "-pendin",	LFLAG,	~PENDIN,	0	},
    { "iexten",		LFLAG,	~IEXTEN,	IEXTEN	},
    { "-iexten",	LFLAG,	~IEXTEN,	0	},
    { NULL,		0,	 0,		0	},
};

char	buff[65536];
char	thisline[258];

#define	NUM_PUSH	5

int	monitor  = 0;
char *filename   = "stdout";
int	width    = 80;
int	carret   = 0;
int	breakTest = FALSE;
int	linefeed = 1;
int	bsize    = 0;
int	speed	 = 0;
int	linecnt  = 0;
int	report   = 0;
int	stty     = 0;

char *speeds[] = 
{
	"default",
	"50 (57600)",
	"75",
	"110",
	"134",
	"150",
	"200",
	"300",
	"600",
	"1200",
	"1800",
	"2400",
	"4800",
	"9600",
	"19200",
	"38400"
};

void usage( int flag );
void process_flags( int ac, char **av );
void process_args( int ac, char **av );
int MyWrite( int file, char *data, int amount );

int main( int ac, char **av )
{
	int	line;
	int	off;
	long	start;
	long	now;
	long	dcnt = 0, count;
	FILE   *console;
	int	file;

	process_args( ac,av );

	if ( strcmp( filename,"stdout" ) )
	{
		file = open( filename, O_WRONLY );
		if ( file==-1 )
		{
			perror( filename );
			exit( 1 );
		}
	}
	else
	{
		file = 1;
	}

	if ( speed || stty )
	{
		if ( ioctl( file, TCGETA, &termio ) == -1 )
		{
			fprintf( stderr,"swirl: could not get port attributes from %s ",filename );
			perror( "TCGETA" );
			exit( 1 );
		}
		if ( speed )
		{
			termio.c_cflag &= ~CBAUD;
			termio.c_cflag |= speed;
		}
		if ( stty )
		{
			int ent;

			for( ent=0; stty_opts[ent].name; ent++ )
			{
				if ( stty_opts[ent].yes )
				{
					*FlagPtrs[ stty_opts[ent].element ] =
						(*FlagPtrs[ stty_opts[ent].element ] & stty_opts[ent].clrbits) | stty_opts[ent].setbits;
				}
			}
		}
		if ( ioctl( file, TCSETA, &termio ) == -1 )
		{
			fprintf( stderr,"swirl: could not set port attributes for %s ",filename );
			perror( "TCSETA" );
			exit( 1 );
		}
	}

	if ( report )
	{
		fprintf( stderr,"Output to:	%s\n",filename );
		if ( linecnt )
			fprintf( stderr,"Line count:	%d\n",linecnt );
		else
			fprintf( stderr,"Line count:	Unlimited\n" );
		fprintf( stderr,"Line length:	%d\n",width );
		fprintf( stderr,"Speed:		%s\n",speeds[speed] );
		fprintf( stderr,"Monitoring:	%s\n",monitor==0?"disabled":monitor==1?"stderr":monitor==2?"console":"both stderr & console");
		if ( !carret )
			if ( !linefeed )
				fprintf( stderr,"Line term.:	Neither CR nor LF\n" );
		else
			fprintf( stderr,"Line term.:	LF\n" );
		else
			if ( !linefeed )
			fprintf( stderr,"Line term.:	CR\n" );
		else
			fprintf( stderr,"Line term.:	CR/LF\n" );
		if ( bsize )
			fprintf( stderr,"Output buffer:	%d\n",bsize );
		else
			fprintf( stderr,"Output buffer:	1 line\n" );
	}

	time( &start );
	
	if ( breakTest )
	{
		fprintf( stderr, "Break is being sent\n" );
		tcsendbreak( file, 2);	// 2 * T (where 0.25 <= T < 0.5 sec)
		return(0);
	}

	
	for( line=0; !linecnt || (line < linecnt) ;line++ )
	{
		off = sprintf( thisline,"%d: ",line );
		for( ; off<width-1; off++ )
			thisline[off] = ((line+off)%95)+' ';
		if ( linefeed )
			thisline[off++] = '\n';
		if ( carret )
			thisline[off++] = '\r';
		if ( bsize )
			dcnt += MyWrite( file,thisline,off );
		else
		{
			count = write( file,thisline,off );
			if ( count < 0 )
				perror( "write" );
			else
				dcnt += count;
		}
		if ( monitor && !(line & 0xff) )
		{
			time( &now );
			if ( now-start > 30 )
			{
				if ( monitor & 2 )
				{
					console = fopen( "/dev/console","w" );
#if 0
						fprintf( console,"%d bytes in %d seconds (%f bps) to %s\n",dcnt,now-start,
						((double)dcnt/(double)(now-start)),filename );
#endif
					fclose( console );
				}
				if ( monitor & 1 )
#if 0
					fprintf( stderr,"%d bytes in %d seconds (%f bps) to %s\n",dcnt,now-start,
						((double)dcnt/(double)(now-start)),filename );
#endif
				start = now;
				dcnt = 0;
			}
		}
	}
	return(0);
	
}

int MyWrite( int file, char *data, int amount )
{
    int	rem = amount;
    int res, i;
    static int buff_ptr = 0;

    while ( rem )
    {
	if ( buff_ptr+rem >= bsize )
	{
	    memcpy(buff+buff_ptr,data,bsize-buff_ptr);
	    data += bsize-buff_ptr;
	    rem  -= bsize-buff_ptr;
	    if ( (res=write(file,buff,bsize)) != bsize )
	    {
		fprintf(stderr,"swirl: Parital write - %d/%d\n",res,bsize);
		if ( res==-1 )
		    return -1;
		for ( i=res; i<bsize; i++ )
		    buff[i-res] = buff[i];
		buff_ptr = bsize-res;
	    }
	    else
		buff_ptr = 0;
	}
	else
	{
	    memcpy(buff+buff_ptr,data,rem);
	    data+=rem;
	    buff_ptr+=rem;
	    rem=0;
	    break;
	}
    }
    return amount;
}

void process_args( int ac, char **av )
{
    extern int optind;

    process_flags(ac,av);

    if ( !stty )
    {
        if ( optind != ac )
	{
            fprintf(stderr,"swirl: extra arguments on command line %d/%d\n",optind,ac);
	    usage(0);
        }
        return;
    }

    if ( optind == ac )
    {
        fprintf(stderr,"swirl: no stty options specified.\n");
	usage(0);
    }

    while ( optind != ac )
    {
        int ent;

        for ( ent = 0; stty_opts[ent].name; ent++ )
	    if ( strcmp( av[optind], stty_opts[ent].name ) == 0 )
	    {
		stty_opts[ent].yes = 1;
		break;
	    }

	if ( !stty_opts[ent].name )
        {
	    fprintf(stderr,"swirl: unrecognised stty option `%s'\n", av[optind] );
	    usage(0);
        }
	optind++;
    }
}

void process_flags( int ac, char **av )
{
	extern char *optarg;
//	extern int optind;

	if ( ac==2 )
	{
	    if ( strcmp(av[1],"help")==0 || strcmp(av[1],"-help") == 0 )
		usage(1);
	}

	while ( 1 )
	{
		/*
		**	-M		monitor progress to /dev/console
		**	-m		monitor progress to stderr
		**	-l		flip linefeed switch
		**	-c		flip carret switch
		**  -k		send break to device.
		**	-f name		set filename to swirl too
		**	-w len		set swirl line width
		**	-B size		set block size for write() calls
		** 	-b speed	set baudrate
		** 	-n count	set number of lines to swirl
		** 	-v		report settings to stderr
		**	-s stty opts... set stty style settings
		*/
		int opt = getopt(ac,av,"Mmlckf:w:B:b:n:pPh:vs");

		if ( opt == -1 )
		    return;

		switch ( opt )
		{
		case 's':
			stty = 1;
			return;
			break;
		case 'M':
			monitor |= 2;
			break;
		case 'm':
			monitor |= 1;
			break;
		case 'l':
			linefeed = !linefeed;
			break;
		case 'c':
			carret = !carret;
			break;

		case 'k':
			breakTest = TRUE;
			break;

		case 'f':
			filename = optarg;
			break;
		case 'w':
			width = atoi(optarg);
			if ( width < 1 || width > 256 )
			{
				fprintf(stderr,"swirl: width must be in range 1..256\n");
				usage(0);
			}
			break;
		case 'B':
			bsize = atoi(optarg);
			if ( bsize < 1 || bsize > 65536 )
			{
				fprintf(stderr,"swirl: block size must be in range 1..65536\n");
				usage(0);
			}
			break;
		case 'b':
			speed = atoi(optarg);
			switch ( speed )
			{
			case 50:
			case 57600:	speed = B50;	break;
			case 75:	speed = B75;	break;
			case 110:	speed = B110;	break;
			case 134:	speed = B134;	break;
			case 150:	speed = B150;	break;
			case 200:	speed = B200;	break;
			case 300:	speed = B300;	break;
			case 600:	speed = B600;	break;
			case 1200:	speed = B1200;	break;
			case 1800:	speed = B1800;	break;
			case 2400:	speed = B2400;	break;
			case 4800:	speed = B4800;	break;
			case 9600:	speed = B9600;	break;
			case 19200:	speed = B19200;	break;
			case 38400:	speed = B38400;	break;
			default:
			    fprintf(stderr,"swirl: invalid baud rate. Valid speeds are 75, 110, 134, 150, 200,\n");
			    fprintf(stderr,"       300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400\n");
			    usage(0);
			}
			break;
		case 'n':
			linecnt = atoi(optarg);
			if ( linecnt < 1 )
			{
			    fprintf(stderr,"swirl: invalid line count - minimum is 1 line\n");
			    usage(0);
			}
			break;
		case 'v':
			report = 1;
			break;
		case -1:
			return;
		default:
			usage(0);
			break;
		}
	}
}

void usage( int flag )
{
    fprintf(stderr,"Usage:\n");
    fprintf(stderr,"swirl [-m|M] [-l] [-c] [-k] [-f output device name] [-w width] [-B blocksize]\n");
    fprintf(stderr,"      [-n count] [-v] [-b speed] [-s stty settings]\n");
    fprintf(stderr,"-M: monitor progress on /dev/console\n");
    fprintf(stderr,"-m: monitor progress on stderr\n");
    fprintf(stderr,"-l: toggle line feed at end of each line (default is ON)\n");
    fprintf(stderr,"-c: toggle carriage return at end of each line (default is OFF)\n");
    fprintf(stderr,"-k: send break to output device\n");
    fprintf(stderr,"-w: use specified line width (default is 80)\n");
    fprintf(stderr,"-B: write output in 'n' byte blocks (default is line width)\n");
    fprintf(stderr,"-b: set baud rate of port before starting output\n");
    fprintf(stderr,"-n: only swirl for `count' lines.\n");
    fprintf(stderr,"-v: report on settings before swirling\n\n");
    fprintf(stderr,"-s: set stty type settings (must be last option)\n");
    fprintf(stderr,"For -l and -c remember that the line discipline cooking may add or ignore\n");
    fprintf(stderr,"CR/LF characters.\n");
    if ( flag )
    {
        int ent;
        int off;

        fprintf(stderr,"stty options supported are:\n");

        for ( ent=0; stty_opts[ent].name; ent++ )
        {
	    fprintf(stderr,"%s",stty_opts[ent].name);
	    if ( ( ent & 7 ) == 7 )
	        fputc( '\n', stderr );
	    else
	        for ( off=strlen( stty_opts[ent].name ); off<10; off++ )
		    fputc( ' ', stderr );
        }

        fputc( '\n', stderr );

    }
    else
        fprintf(stderr,"`swirl -help' will print a list of available stty options\n");
    exit(1);
}
