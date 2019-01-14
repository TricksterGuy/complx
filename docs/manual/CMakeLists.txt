# 
# Input and output files
# 

set(pdf    "${CMAKE_CURRENT_BINARY_DIR}/complx-tools.pdf")

# 
# Deployment
# 

# PDF file
install(FILES ${pdf}
    DESTINATION "${INSTALL_DOC}"
    COMPONENT docs
)
