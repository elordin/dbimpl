#pragma once

class BufferFrame {
    private:
	int pageNo;
	bool locked = false;
	void* data;

	static int countPages;
    public:
        BufferFrame(void* dataForFrame);

        void* getData();

        ~BufferFrame();
};
