# Register components to build system
idf_component_register(SRCS "main.c" INCLUDE_DIRS "." REQUIRES ${requires})

# Note:  If no requirements are specified, the default behavior is 
#       to add all components in the build to the requirements list.