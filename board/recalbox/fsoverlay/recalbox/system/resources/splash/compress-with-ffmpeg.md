# Optimal compression with ffmpeg

## Standard 1080p

```bash
ffmpeg -y -i "$INPUT" \
        -c:v libx264 -b:v 2000k -pass 1 \
        -c:a aac -b:a 128k -f mp4 /dev/null && \
ffmpeg -i "$INPUT" \
        -c:v libx264 -b:v 2000k -pass 2 \
        -c:a aac -b:a 128k "$OUTPUT"

```


## 240p

```bash
ffmpeg -y -i "$INPUT" \
  -vf "scale=426:240,crop=320:240:53:0" \
        -c:v libx264 -b:v 50k -pass 1 \
        -c:a aac -b:a 128k -f mp4 /dev/null && \
ffmpeg -i "$INPUT" \
  -vf "scale=426:240,crop=320:240:53:0" \
        -c:v libx264 -b:v 50k -pass 2 \
        -c:a aac -b:a 128k "$OUTPUT"

```
