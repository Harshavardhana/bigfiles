SET (libbigobjects_C bigobjects.c bigobjects-private.c driver.c uri.c)
SET (libbigobjects_h bigobjects.h bigobjects-private.h driver.h uri.h common.h)

ADD_LIBRARY( bigobjects SHARED ${libbigobjects_C} ${libbigobjects_h} )
SET_TARGET_PROPERTIES( bigobjects PROPERTIES VERSION 0.0.0 SOVERSION 0 )
TARGET_LINK_LIBRARIES( bigobjects )
