if [ -d "SuperChat/" ]; then
	rm -rf SuperChat
fi

git clone https://github.com/bdavisCSE3320/SuperChat.git
rm -rf SuperChat/.git/