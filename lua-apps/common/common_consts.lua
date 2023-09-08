--status_t--
OK = 1;
ERROR = 0;

E_OK = 1;
E_ERROR = 0;
E_PART_OK = 100;
E_TIMEOUT = -1;
E_INNER_ERROR = -2;

--SliceFileName flags
FN_SINGLE   = 0x00000001;
FN_EXT      = 0x00000002;
FN_PATH     = 0x00000004;
FN_FILENAME = (FN_SINGLE|FN_EXT);
FN_FULLNAME = (FN_PATH|FN_FILENAME);

--folder traverse events
EVENT_BEGIN_DIR = 1;
EVENT_END_DIR = 2;
EVENT_SINGLE_FILE = 3;

--tcp acceptor & connector event
EVENT_NEW_CLIENT = 1;
EVENT_STOP = 2;
EVENT_CONNECTED = 1;

--cmdline key types    
TYPE_KEY = 1;
TYPE_VALUE = 2;
TYPE_KEY_VALUE = 3;
TYPE_KEY_EQUAL_VALUE = 4;
TYPE_KEY_SPACE_VALUE = 5;

--cmdline key options
MUST = 1;
OPTIONAL = 2;

DEP_OP_AND = 0;
DEP_OP_OR = 1;
DEP_OP_NOT = 2;
----
LBUF_SIZE = 32*1024;
EOL = "\r\n";

CHAR_SPACE = 32;
CHAR_UNDERLINE = 95;
CHAR_A = 65;
CHAR_Z = 90;
CHAR_a = 97;
CHAR_z = 122;
CHAR_0 = 48;
CHAR_9 = 57;
--copy_single_file modes
MODE_OVERWRITE = 0x00000001;
MODE_CHECKSAME = 0x00000002;
MODE_USE_LOG = 0x00000004;
MODE_COPY_LINK = 0x00000010;
MODE_PART_COPY = 0x00000020;

--message peer data types
BODY_TYPE_STRING = 0x01<<4;
BODY_TYPE_BSON = 0x02<<4;
BODY_TYPE_RAW = 0x03<<4;
BODY_TYPE_JSON = 0x04<<4;

--message peer message types
MSG_TYPE_REQUEST =  0x01;
MSG_TYPE_RESPONSE  = 0x02;
MSG_TYPE_SIGNAL = 0x03;

--message peer type masks
BODY_TYPE_MASK = 0x000000f0;
MSG_TYPE_MASK = 0x0000000f;

--bson traverse events
EVENT_BEGIN_DOCUMENT = 1;
EVENT_END_DOCUMENT = 2;
EVENT_SINGLE_VALUE = 3;

--bson data types
BSON_TYPE_DOUBLE=0x01;
BSON_TYPE_STRING=0x02;
BSON_TYPE_DOCUMENT=0x03;
BSON_TYPE_ARRAY=0x04;
BSON_TYPE_BINARY=0x05;
BSON_TYPE_BOOLEAN=0x08;
BSON_TYPE_INT32=0x10;
BSON_TYPE_INT64=0x12;

---lexer types--
TOKEN_UNKNOWN = 0;
TOKEN_MULTI_LINE_COMMENTS = 1;
TOKEN_SINGLE_LINE_COMMENTS = 2;
TOKEN_PUNCTUATION = 3;
TOKEN_PREPROCESS = 4;
TOKEN_STRING = 5;
TOKEN_NUMBER = 6;
TOKEN_NORMAL_WORD = 7;
TOKEN_EMPTY = 8;
TOKEN_KEY_WORD = 9;
TOKEN_XML_TAG = 10;
TOKEN_XML_ATTRIBUTE = 11;
TOKEN_XML_NORMAL_TEXT = 12;

--GetHWnd Types--
HWND_EDIT_BOX    =	 1
HWND_MAIN_FORM   =	 2
HWND_EMBEDDED_UI =   3

--app event type
APP_EVENT_EMBEDDED_UI_WINDOW_RESIZED = 1;
APP_EVENT_EMBEDDED_UI_WINDOW_MESSAGE = 2;

--hex to binary table--
HEX_TO_BIN_TABLE={
    [48] = 0x00,
    [49] = 0x01,
    [50] = 0x02,
    [51] = 0x03,
    [52] = 0x04,
    [53] = 0x05,
    [54] = 0x06,
    [55] = 0x07,
    [56] = 0x08,
    [57] = 0x09,
    [97] = 0x0a,
    [98] = 0x0b,
    [99] = 0x0c,
    [100] = 0x0d,
    [101] = 0x0e,
    [102] = 0x0f,
	[65] = 0x0a,
    [66] = 0x0b,
    [67] = 0x0c,
    [68] = 0x0d,
    [69] = 0x0e,
    [70] = 0x0f,
};
