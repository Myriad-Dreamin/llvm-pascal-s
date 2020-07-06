
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)

#LLVM_TARGET(AArch64)
#LLVM_TARGET(AMDGPU)
#LLVM_TARGET(ARM)
#LLVM_TARGET(AVR)
#LLVM_TARGET(BPF)
#LLVM_TARGET(Hexagon)
#LLVM_TARGET(Lanai)
#LLVM_TARGET(Mips)
#LLVM_TARGET(MSP430)
#LLVM_TARGET(NVPTX)
#LLVM_TARGET(PowerPC)
#LLVM_TARGET(RISCV)
#LLVM_TARGET(Sparc)
#LLVM_TARGET(SystemZ)
#LLVM_TARGET(WebAssembly)
#LLVM_TARGET(X86)
#LLVM_TARGET(XCore)

#aarch64asmparser aarch64codegen aarch64desc aarch64disassembler aarch64info aarch64utils
#        aggressiveinstcombine
#        amdgpuasmparser amdgpucodegen amdgpudesc amdgpudisassembler amdgpuinfo amdgpuutils
#        analysis
#        armasmparser armcodegen armdesc armdisassembler arminfo armutils
#        asmparser asmprinter
#        avrasmparser avrcodegen avrdesc avrdisassembler avrinfo
#        binaryformat bitreader bitstreamreader bitwriter
#        bpfasmparser bpfcodegen bpfdesc bpfdisassembler bpfinfo
#        cfguard codegen core coroutines coverage debuginfocodeview debuginfodwarf debuginfogsym debuginfomsf debuginfopdb demangle dlltooldriver dwarflinker executionengine extensions frontendopenmp fuzzmutate globalisel hexagonasmparser hexagoncodegen hexagondesc hexagondisassembler hexagoninfo instcombine instrumentation interpreter ipo irreader lanaiasmparser lanaicodegen lanaidesc lanaidisassembler lanaiinfo libdriver lineeditor linker lto mc mca mcdisassembler mcparser mipsasmparser mipscodegen mipsdesc mipsdisassembler mipsinfo mirparser msp430asmparser msp430codegen msp430desc msp430disassembler msp430info native nativecodegen nvptxcodegen nvptxdesc nvptxinfo objcarcopts object objectyaml option orcerror passes powerpcasmparser powerpccodegen powerpcdesc powerpcdisassembler powerpcinfo profiledata remarks riscvasmparser riscvcodegen riscvdesc riscvdisassembler riscvinfo riscvutils runtimedyld scalaropts selectiondag sparcasmparser sparccodegen sparcdesc sparcdisassembler sparcinfo support symbolize systemzasmparser systemzcodegen systemzdesc systemzdisassembler systemzinfo tablegen target textapi transformutils vectorize webassemblyasmparser webassemblycodegen webassemblydesc webassemblydisassembler webassemblyinfo windowsmanifest x86asmparser x86codegen x86desc x86disassembler x86info xcorecodegen xcoredesc xcoredisassembler xcoreinfo xray

set(llvm_components support core irreader mc native)

#x86asmparser x86codegen x86desc x86disassembler x86info

llvm_map_components_to_libnames(LLVMLIBS ${llvm_components})

#add_definitions(${LLVM_DEFINITIONS})

file(GLOB PASCAL_S_TARGET_LLVM_SOURCE_FILES **.cpp)

add_library(target-llvm ${PASCAL_S_TARGET_LLVM_SOURCE_FILES})
#target_include_llvm(target-llvm)
target_link_libraries(target-llvm fmt ${LLVMLIBS})


#add_subdirectory(test)
