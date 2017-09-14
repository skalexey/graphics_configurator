message("-=-=-=- SocialQuantumTest -=-=-=-")

cmake_minimum_required(VERSION 2.6)

project(SocialQuantumTest)

file(GLOB SRC src/*.cpp src/*.h)
file(GLOB CFG cfg/*.ini)

add_executable(SocialQuantumTest ${SRC} ${CFG})


install(TARGETS SocialQuantumTest
	RUNTIME DESTINATION .
	COMPONENT SocialQuantumTest)

include_directories(/Projects/include include)
link_directories(lib)