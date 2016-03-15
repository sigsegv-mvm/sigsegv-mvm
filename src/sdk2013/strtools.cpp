int V_vsnprintfRet( char *pDest, int maxLen, const char *pFormat, va_list params, bool *pbTruncated )
{
	Assert( maxLen > 0 );
	AssertValidWritePtr( pDest, maxLen );
	AssertValidStringPtr( pFormat );

	int len = _vsnprintf( pDest, maxLen, pFormat, params );

	if ( pbTruncated )
	{
		*pbTruncated = ( len < 0 || len >= maxLen );
	}

	if	( len < 0 || len >= maxLen )
	{
		len = maxLen;
		pDest[maxLen-1] = 0;
	}

	return len;
}
