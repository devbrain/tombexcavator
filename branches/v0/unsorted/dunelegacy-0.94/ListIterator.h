class ListIterator
{
public:
	ListIterator();
	~ListIterator();
	inline void replaceCurrent(void *dd) { current->dData = dd; }
	inline bool isEmpty() { return first == NULL; }
	inline bool atEnd() { return (current->next==NULL); }
	inline bool currentNotNull() { return (current != NULL); }
	inline int getNumElements() { return numElements; }
	void reset();
	inline void nextLink() { current = current->next; }
	void insertFirst(void* dd);
	void insertLast(void* dd);
	void restoreCurrent();
	void saveCurrent();

	inline void* getFirstElement() { return first->dData; }
	inline void* getCurrent() { return current->dData; }
	void* findElement(void* element);
	void* getElement(int elementNum);
	void* getRandomElement();
	void* getRandomElementNotCurrent();
	void* removeCurrent();
	void* removeElement(void* element);
	void* removeFirstElement();
	void clearList();

private:
	Link	*first,
			*current,
			*last,
			*savedCurrent;

	int  numElements;
};

