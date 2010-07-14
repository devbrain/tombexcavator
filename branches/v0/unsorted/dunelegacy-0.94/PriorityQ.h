class PriorityQ
{
public:
	PriorityQ(int mx);            // constructor
	~PriorityQ();
	void trickleDown(int index);
	void trickleUp(int index);
	bool insertNode(TerrainClass* newNode);
	bool insertObject(void* newObject);
	TerrainClass* findNodeWithKey(COORDTYPE* location);
	TerrainClass* removeNode();           // delete item with max key
	void* remove();           // delete item with max key
	TerrainClass* removeNodeWithKey(COORDTYPE* location);

	inline bool isEmpty() { return (currentSize == 0); }
private:
	int currentSize,	// size of array
		maxSize;	// number of items in array

	TerrainClass** heapArray;
};

