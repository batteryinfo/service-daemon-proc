project(SAMPLE_server)

# Find required modules
# PKG_CHECK_MODULES: used to check for given modules
# PKG_SEARCH_MODULE: used to check for given modules and uses the first working one
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(rpkgs-dlog REQUIRED dlog)
PKG_CHECK_MODULES(rpkgs-capi REQUIRED capi-network-bluetooth capi-system-info)
PKG_CHECK_MODULES(rpkgs-ecore REQUIRED 
	capi-appfw-application
	capi-appfw-app-manager
	capi-system-runtime-info
	elementary
	notification
	libprivilege-control
	ecore
	ecore-file
	ecore-x
	eina
	sysman
	vconf
	heynoti
	pmapi
	tapi
	dlog
	syspopup-caller
	notification
	device-node
	libsmack
	libsystemd-daemon
)

# Add CFLAGS used for required modules
ADD_DEFINITIONS(${rpkgs-dlog_CFLAGS})
ADD_DEFINITIONS(${rpkgs-capi_CFLAGS})
ADD_DEFINITIONS(${rpkgs-ecore_CFLAGS})

# Set include directories
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${rpkgs-dlog_INCLUDE_DIRS}
	${rpkgs-capi_INCLUDE_DIRS}
	)

# Set sources
ADD_EXECUTABLE(${PROJECT_NAME}
	src/main.c
	src/custom.c
	src/ss_log.c
	src/vconf.c
)

# Set LDFLAGS
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${rpkgs-dlog_LDFLAGS} ${rpkgs-capi_LDFLAGS} ${rpkgs-ecore_LDFLAGS} "-ldl -lrt")

# Set CFLAGS
ADD_DEFINITIONS(-Wall -O3) 
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

# Move result binary to the target position(/usr/bin)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
