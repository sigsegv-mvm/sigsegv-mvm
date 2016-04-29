#ifndef _INCLUDE_SIGSEGV_MOD_UTIL_OVERLAY_SHARED_H_
#define _INCLUDE_SIGSEGV_MOD_UTIL_OVERLAY_SHARED_H_


enum OverlayType : uint8_t
{
	/* main NDebugOverlay functions */
	OV_BOX,
	OV_BOX_ANGLES,
	OV_SWEPT_BOX,
	OV_LINE,
	OV_TRIANGLE,
	OV_ENTITY_TEXT,
	OV_ENTITY_TEXT_AT_POSITION,
	OV_GRID,
	OV_TEXT,
	OV_SCREEN_TEXT,
	OV_CROSS3D_EXT,
	OV_CROSS3D_SIZE,
	OV_CROSS3D_ORIENTED_ANG,
//	OV_CROSS3D_ORIENTED_MAT,
//	OV_DRAW_TICK_MARKED_LINE,
//	OV_DRAW_GROUND_CROSSHAIR_OVERLAY,
	OV_HORZ_ARROW,
	OV_YAW_ARROW,
	OV_VERT_ARROW,
	OV_AXIS,
	OV_SPHERE,
	OV_CIRCLE_ANG,
	OV_CIRCLE_AXES,
	OV_SPHERE_ANG,
	
	/* my own additions to NDebugOverlay */
	OV_CLEAR,
	OV_LINE_ALPHA,
	
	/* for bandwidth testing */
	OV_BANDWIDTH_TEST,
	
	OV_MAX,
};
constexpr int OVERLAY_TYPE_BITS = 5;
static_assert((1 << OVERLAY_TYPE_BITS) >= OV_MAX, "OVERLAY_TYPE_BITS insufficient");

constexpr int ENTITY_ID_BITS = 12;

constexpr float MAX_OVERLAY_DIST_SQR = 90000000.0f;


constexpr uint16_t OVERLAY_PORT = 30000;


#endif
