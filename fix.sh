#!/usr/bin/env bash
mkdir -p "$HOME/bin"
cat > "$HOME/bin/libtoolize" <<'EOF'
#!/bin/bash
exec /bin/bash /usr/bin/libtoolize "$@"
EOF
chmod +x "$HOME/bin/libtoolize"
export PATH="$HOME/bin:$PATH"
pip install jinja2
