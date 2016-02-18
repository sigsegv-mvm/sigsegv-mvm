// No spaces in event names, max length 32
// All strings are case sensitive
//
// valid data key types are:
//   string : a zero terminated string
//   bool   : unsigned int, 1 bit
//   byte   : unsigned int, 8 bit
//   short  : signed int, 16 bit
//   long   : signed int, 32 bit
//   float  : float, 32 bit
//   local  : any data, but not networked to clients
//
// following key names are reserved:
//   local      : if set to 1, event is not networked to clients
//   unreliable : networked, but unreliable
//   suppress   : never fire this event
//   time	: firing server time
//   eventid	: holds the event ID

"SigsegvEvents"
{
	// NDebugOverlay
//	"overlay_Box"
//	{
//		origin_x   float
//		origin_y   float
//		origin_z   float
//		mins_x     float
//		mins_y     float
//		mins_z     float
//		maxs_x     float
//		maxs_y     float
//		maxs_z     float
//		r          short
//		g          short
//		b          short
//		a          short
//		flDuration float
//	}
	
	// IVDebugOverlay
	"overlay_AddEntityTextOverlay"
	{
		ent_index   short
		line_offset short
		duration    float
		r           short
		g           short
		b           short
		a           short
		text        string
	}
//	
//	"overlay_AddBoxOverlay"
//	{
//		origin_x      float
//		origin_y      float
//		origin_z      float
//		mins_x        float
//		mins_y        float
//		mins_z        float
//		max_x         float
//		max_y         float
//		max_z         float
//		orientation_x float
//		orientation_y float
//		orientation_z float
//		r             short
//		g             short
//		b             short
//		a             short
//		duration      float
//	}
//	
//	"overlay_AddTriangleOverlay"
//	{
//		p1_x        float
//		p1_y        float
//		p1_z        float
//		p2_x        float
//		p2_y        float
//		p2_z        float
//		p3_x        float
//		p3_y        float
//		p3_z        float
//		r           short
//		g           short
//		b           short
//		a           short
//		noDepthTest bool
//		duration    float
//	}
//	
//	"overlay_AddLineOverlay"
//	{
//		origin_x    float
//		origin_y    float
//		origin_z    float
//		dest_x      float
//		dest_y      float
//		dest_z      float
//		r           short
//		g           short
//		b           short
//		noDepthTest bool
//		duration    float
//	}
//	
//	"overlay_AddTextOverlay"
//	{
//		origin_x    float
//		origin_y    float
//		origin_z    float
//		line_offset short
//		duration    float
//		text        string
//	}
//	
//	"overlay_AddScreenTextOverlay"
//	{
//		flXPos     float
//		flYPos     float
//		flDuration float
//		r          short
//		g          short
//		b          short
//		a          short
//		text       string
//	}
//	
//	"overlay_AddSweptBoxOverlay"
//	{
//		start_x    float
//		start_y    float
//		start_z    float
//		end_x      float
//		end_y      float
//		end_z      float
//		mins_x     float
//		mins_y     float
//		mins_z     float
//		max_x      float
//		max_y      float
//		max_z      float
//		angles_x   float
//		angles_y   float
//		angles_z   float
//		r          short
//		g          short
//		b          short
//		a          short
//		flDuration float
//	}
//	
//	"overlay_AddGridOverlay"
//	{
//		origin_x float
//		origin_y float
//		origin_z float
//	}
//	
//	"overlay_AddTextOverlayRGB"
//	{
//		origin_x    float
//		origin_y    float
//		origin_z    float
//		line_offset short
//		duration    float
//		r           short
//		g           short
//		b           short
//		a           short
//		text        string
//	}
//	
//	"overlay_AddLineOverlayAlpha"
//	{
//		origin_x    float
//		origin_y    float
//		origin_z    float
//		dest_x      float
//		dest_y      float
//		dest_z      float
//		r           short
//		g           short
//		b           short
//		a           short
//		noDepthTest bool
//		duration    float
//	}
//	
//	"overlay_AddBoxOverlay2"
//	{
//		origin_x      float
//		origin_y      float
//		origin_z      float
//		mins_x        float
//		mins_y        float
//		mins_z        float
//		max_x         float
//		max_y         float
//		max_z         float
//		orientation_x float
//		orientation_y float
//		orientation_z float
//		faceColor_r   short
//		faceColor_g   short
//		faceColor_b   short
//		faceColor_a   short
//		edgeColor_r   short
//		edgeColor_g   short
//		edgeColor_b   short
//		edgeColor_a   short
//		duration      float
//	}
}
