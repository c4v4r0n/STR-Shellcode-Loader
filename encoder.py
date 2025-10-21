import sys
import textwrap

CHUNK_SIZE = 300

def bytes_to_unicode_escape(data: bytes) -> str:
    # Convert each byte to \xHH format and wrap as a wide string (L"")
    return ''.join(f'{b:02X}' for b in data)

def make_string_table_block(start_id, strings):
    lines = ['STRINGTABLE', 'BEGIN']
    for i, s in enumerate(strings):
        lines.append(f'    {start_id + i} L"{s}"')
    lines.append('END\n')
    return '\n'.join(lines)

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.bin>")
        sys.exit(1)

    blob = open(sys.argv[1], 'rb').read()
    chunks = [blob[i:i+CHUNK_SIZE] for i in range(0, len(blob), CHUNK_SIZE)]

    lines = []
    for i in range(0, len(chunks), 16):
        block_chunks = chunks[i:i+16]
        block_id = (i // 16) * 16 + 1
        escaped = [bytes_to_unicode_escape(c) for c in block_chunks]
        lines.append(make_string_table_block(block_id, escaped))

    with open('resources/strings.rc', 'w', encoding='utf-16le') as f:
        f.write('\n'.join(lines))

if __name__ == '__main__':
    main()