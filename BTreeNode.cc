#include "BTreeNode.h"

using namespace std;

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ return 0; }

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ return 0; }

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ return 0; }

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ return 0; }

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ return 0; }

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ return 0; }

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ return 0; }

/********************************* Non Leaf Node ************************************/
BTNonLeafNode::BTNonLeafNode() {
	key_count = 0;
	bzero(buffer, PageFile::PAGE_SIZE);
}

struct BTNonLeafNode::non_leaf_entry {
	non_leaf_entry(int Key, PageId Pid):key(Key),pid(Pid){}
	int key;
	PageId pid;
};

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	return pf.read(pid, buffer); 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ 
	return pf.write(pid, buffer); 
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 
	int count = 0;
	int max_num_entry = (PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(non_leaf_entry));
	non_leaf_entry* entry_ptr = (non_leaf_entry*) buffer;
	while(count < max_num_entry){
		if(entry_ptr->key == 0) break;
		count++;
		entry_ptr++;
	}
	return count;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 
	int cur_entry_count = getKeyCount();
	int max_num_entry = (PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(non_leaf_entry));
	if(cur_entry_count >= max_num_entry) return RC_NODE_FULL;
	//buffer_ptr points to the head of first non_leaf_entry
	char* buffer_ptr = buffer + PageFile::PAGE_SIZE - max_num_entry*sizeof(non_leaf_entry);
	int i;
	for(i = 0; i < max_num_entry; i++){
		int* cur_key_ptr;
		cur_key_ptr = (int*)buffer_ptr;
		if(*cur_key_ptr == 0 || key <= *cur_key_ptr) break;
        buffer_ptr = (char*)buffer_ptr;
        buffer_ptr+= sizeof(non_leaf_entry);
	}
	//new_entry is the entry to insert
	non_leaf_entry* new_entry = new non_leaf_entry(key, pid);
	//shift_ptr points to the last entry in the buffer
	non_leaf_entry* shift_ptr = (non_leaf_entry*)buffer + cur_entry_count;
	char* temp_buffer = new char[PageFile::PAGE_SIZE];
	//shift [buffer_ptr, shift_ptr+8] to sizeof(non_leaf_entry) right
	size_t to_shift_size = (char*)shift_ptr - buffer_ptr + 8;
	memcpy(temp_buffer+sizeof(non_leaf_entry), buffer_ptr, to_shift_size);
	memcpy(temp_buffer, new_entry, sizeof(non_leaf_entry));
	memcpy(buffer_ptr, temp_buffer, to_shift_size+sizeof(non_leaf_entry));
	key_count++;
	delete[] temp_buffer;
	delete new_entry;
	return 0; 
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ 
	int cur_entry_count = getKeyCount();
	int max_num_entry = (PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(non_leaf_entry));

	if(cur_entry_count < max_num_entry) return RC_INVALID_FILE_FORMAT;

	if(sibling.getKeyCount()!=0) return RC_INVALID_ATTRIBUTE;

	int half_count = (cur_entry_count+1)/2;

	int half_index = half_count*sizeof(non_leaf_entry)+8;

	int key1 = *((int*)(buffer + half_index - 8));
	int key2 = *((int*)(buffer + half_index));

	if(key < key1) {
		memcpy(sibling.buffer+8, buffer + half_index, PageFile::PAGE_SIZE - half_index);
		sibling.key_count = cur_entry_count - half_count;

		midKey = *((int*)(buffer + half_index - 8));
		memcpy(sibling.buffer, buffer+half_index-4, sizeof(PageId));

		fill(buffer+half_index-8, buffer + PageFile::PAGE_SIZE, 0);
		key_count = half_count - 1;

		insert(key, pid);
	}
	else if(key > key2){
		memcpy(sibling.buffer+8, buffer + half_index + 8, PageFile::PAGE_SIZE - half_index - 8);
		sibling.key_count = cur_entry_count - half_count -1;

		midKey = *((int*)(buffer + half_index));
		memcpy(sibling.buffer, buffer+half_index+4, sizeof(PageId));

		fill(buffer + half_index, buffer + PageFile::PAGE_SIZE, 0);
		key_count = half_count;

		sibling.insert(key, pid);
	}
	else{
		memcpy(sibling.buffer + 8, buffer + half_index, PageFile::PAGE_SIZE - half_index);
		sibling.key_count = cur_entry_count - half_count;

		fill(buffer+half_index, buffer+PageFile::PAGE_SIZE, 0);
		key_count = half_count;

		midKey = key;

		memcpy(sibling.buffer, &pid, sizeof(PageId));

	}
	return 0; 
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ 
	int cur_entry_count = getKeyCount();
    int max_num_entry = (PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(non_leaf_entry));
	char* buffer_ptr = buffer + PageFile::PAGE_SIZE - max_num_entry*sizeof(non_leaf_entry);
	int i;
	for(i=8; i < (cur_entry_count+1)*sizeof(non_leaf_entry); i+=sizeof(non_leaf_entry)){
		int* cur_key_ptr;
		cur_key_ptr = (int*)buffer_ptr;
		if(i==8 && *cur_key_ptr > searchKey){
			pid = *((PageId*)buffer);
			return 0;
		}
		else if(*cur_key_ptr > searchKey){
			pid = *((PageId*)buffer_ptr-1);
			return 0;
		}
        buffer_ptr = (char*)buffer_ptr;
        buffer_ptr+= sizeof(non_leaf_entry);
	}
	pid = *((PageId*)buffer_ptr-1);
	return 0; 
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
	bzero(buffer, PageFile::PAGE_SIZE);

    char* tmp = buffer;
    memcpy(tmp, &pid1, sizeof(PageId));
    
    RC r = insert(key, pid2);
	return r;
 }
