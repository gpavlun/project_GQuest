#! /bin/bash
echo "Project layout:" > architecture.md
tree ../. -a -I '.git|build/*|bin/*|.gitignore|*.o|*.d|*.a|world_data/*' >> architecture.md

echo "" >> architecture.md
echo "Lines of code by file:" >> architecture.md

find ../ \( -name "*.c" -o -name "*.h" \) -exec wc -l {} + >> architecture.md