#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <cm.h>

static FILE *_CMmsgStreamSysError = (FILE *) NULL;
static FILE *_CMmsgStreamAppError = (FILE *) NULL;
static FILE *_CMmsgStreamUsrError = (FILE *) NULL;
static FILE *_CMmsgStreamWarning  = (FILE *) NULL;
static FILE *_CMmsgStreamDebug    = (FILE *) NULL;
static FILE *_CMmsgStreamInfo     = (FILE *) NULL;

static bool  _CMmsgStatusSysError = true;
static bool  _CMmsgStatusAppError = true;
static bool  _CMmsgStatusUsrError = true;
static bool  _CMmsgStatusWarning  = true;
static bool  _CMmsgStatusDebug    = true;
static bool  _CMmsgStatusInfo     = true;

static size_t _CMmsgIndentLevel [] = { 0, 0, 0, 0, 0, 0};

bool CMmsgSetStreamFile (CMmsgType msgType, const char *filename) {
	FILE *fp;

	if ((fp = fopen (filename,"w")) == (FILE *) NULL) return (false);
	CMmsgSetStream (msgType, fp);
	return (true);
}

void CMmsgSetStream (CMmsgType msgType, FILE *fp) {
	if (fp != (FILE *) NULL)
		switch (msgType) {
			case CMmsgSysError: _CMmsgStreamSysError = fp; break;
			case CMmsgAppError: _CMmsgStreamAppError = fp; break;
			case CMmsgUsrError: _CMmsgStreamUsrError = fp; break;
			case CMmsgWarning:  _CMmsgStreamWarning  = fp; break;
			case CMmsgDebug:    _CMmsgStreamDebug    = fp; break;
			case CMmsgInfo:     _CMmsgStreamInfo     = fp; break;
		}
}

void CMmsgCloseAllStreams () {
	CMmsgType types [] = { CMmsgSysError, CMmsgAppError, CMmsgUsrError, CMmsgDebug, CMmsgWarning, CMmsgInfo };
	int i;
	for (i = 0;i < sizeof (types) / sizeof (CMmsgType); ++i)
		CMmsgCloseStream (types [i]);
}

void CMmsgCloseStream (CMmsgType msgType) {
	FILE *fp;

	switch (msgType) {
		case CMmsgSysError: fp = _CMmsgStreamSysError;
		case CMmsgAppError: fp = _CMmsgStreamAppError;
		case CMmsgUsrError: fp = _CMmsgStreamUsrError;
		case CMmsgWarning:  fp = _CMmsgStreamWarning;
		case CMmsgDebug:    fp = _CMmsgStreamDebug;
		case CMmsgInfo:     fp = _CMmsgStreamInfo;
	}
	if ((fp != stderr) && (fp != stdout)) fclose (fp);
}

void CMmsgSetStatus (CMmsgType msgType, bool status) {
	switch (msgType)
	{
		case CMmsgSysError: _CMmsgStatusSysError = status;
		case CMmsgAppError: _CMmsgStatusAppError = status;
		case CMmsgUsrError: _CMmsgStatusUsrError = status;
		case CMmsgWarning:  _CMmsgStatusWarning  = status;
		case CMmsgDebug:    _CMmsgStatusDebug    = status;
		case CMmsgInfo:     _CMmsgStatusInfo     = status;
	}
}

int CMmsgPrint (CMmsgType msgType, const char *format, ...) {
	int i, ret = 0;
	FILE *fp = (FILE *) NULL;
	va_list ap;
	
	switch (msgType) {
		case CMmsgSysError:
			if (_CMmsgStatusSysError) fp = _CMmsgStreamSysError == (FILE *) NULL ?  stderr : _CMmsgStreamSysError; break;
		case CMmsgAppError:
			if (_CMmsgStatusAppError) fp = _CMmsgStreamAppError == (FILE *) NULL ?  stderr : _CMmsgStreamAppError; break;
		case CMmsgUsrError:
			if (_CMmsgStatusUsrError) fp = _CMmsgStreamUsrError == (FILE *) NULL ?  stderr : _CMmsgStreamUsrError; break;
		case CMmsgDebug:
			if (_CMmsgStatusDebug)    fp = _CMmsgStreamDebug    == (FILE *) NULL ?  stdout : _CMmsgStreamDebug;    break;
		case CMmsgWarning:
			if (_CMmsgStatusWarning)  fp = _CMmsgStreamWarning  == (FILE *) NULL ?  stderr : _CMmsgStreamWarning;  break;
		case CMmsgInfo:
			if (_CMmsgStatusInfo)     fp = _CMmsgStreamInfo     == (FILE *) NULL ?  stdout : _CMmsgStreamInfo;     break;
	}
	if (fp != (FILE *) NULL) {
		for (i = 0;i < _CMmsgIndentLevel[msgType]; ++i) fprintf (fp, "   ");
		va_start (ap, format);
		ret = vfprintf (fp, format, ap);
		va_end (ap);
		fflush (fp);
	}
	if (msgType == CMmsgSysError) perror ("Perror:");
	return (ret);
}

void CMmsgIndent (CMmsgType msgType, bool indent) {
	_CMmsgIndentLevel [msgType] =  indent ? _CMmsgIndentLevel [msgType] + 1 : _CMmsgIndentLevel[msgType] - 1;
}
