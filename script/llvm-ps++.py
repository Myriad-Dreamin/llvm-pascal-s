import argparse
import os

src = "examples/hello_world.ps"
dst = "build"

parser = argparse.ArgumentParser()
parser.add_argument('--assembler',
                    default="C:/work/cpp/llvm-pascal-s/cmake-build-debug/src/compiler/compiler.exe",
                    type=str, help='Assembler of Pascal-S')
parser.add_argument('--c-linker', default='g++', type=str, help='Linker')
parser.add_argument('--library', type=str, default=[], help='Linked libraries')
parser.add_argument('--version', type=bool, help='Print Version')
parser.add_argument('--env', type=bool, help='Print the environment of pascal')
parser.add_argument('--use-self-main', type=bool, help='Use user-defined main lib instead of default', default=False)
parser.add_argument('--work-dir', type=str, help='Working directory')
parser.add_argument('--src', type=str, help='Source path', required=True)
parser.add_argument('--out', default="build", type=str, help='Output path')
parser.add_argument('--out-ir', type=bool, help='Output IR Code, enum of {json, yml, fmt, binary, console}')
parser.add_argument('--out-token', type=bool, help='Output tokens, enum of {json, yml, fmt, binary, console}')

args = parser.parse_args()

if isinstance(args.library, str):
    args.library = args.library.split(',')

if not args.use_self_main:
    args.library.append("C:/work/cpp/llvm-pascal-s/lib/libstdps.a")

# print(args)

if not os.path.isfile(args.src):
    print('source file should not be directory')

if not os.path.exists(args.out):
    os.mkdir(args.out)
object_file = os.path.abspath(
    os.path.join(args.out, os.path.splitext(os.path.basename(args.src))[0] + '.o'))
asm = ' '.join([
    args.assembler,
    '--src=' + os.path.abspath(args.src),
    '--o=' + object_file
])
# print(asm)
print('assembling...')
print('generated', object_file)
asm_result = os.popen(asm).readlines()
if len(asm_result) > 0:
    print('\n'.join(asm_result))

link = ' '.join([
                    args.c_linker,
                    '-o', os.path.abspath(
        os.path.join(args.out, os.path.splitext(os.path.basename(args.src))[0])),
                    object_file,
                ] + args.library)
# print(link)
print('linking...')
link_result = os.popen(link).readlines()
if len(link_result) > 0:
    print('\n'.join(link_result))
