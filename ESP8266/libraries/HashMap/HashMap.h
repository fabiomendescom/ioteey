/* $Id: HashMap.h 1198 2011-06-14 21:08:27Z bhagman $
||
|| @author         Alexander Brevig <abrevig@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://alexanderbrevig.com/
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
||
|| @description
|| | Implementation of a HashMap data structure.
|| |
|| | Wiring Cross-platform Library
|| #
||
|| @license Please see cores/Common/License.txt.
||
* 
* 
* THIS WAS MODIFIED BY FABIO MENDES FROM ITS ORIGINAL VERSION ABOVE. SOME FUNCTIONS WERE ADDED TO MAKE IT IOTEEY COMPATIBLE
 * I KEPT THE ORIGINAL COPYRIGHTS ABOVE TO GIVE CREDIT TO THE AUTHORS
* 
*/

#ifndef HASHMAP_H
#define HASHMAP_H

//#include "Countable.h"

//for convenience
//#define CreateHashMap(hashM, ktype, vtype, capacity) HashMap<ktype,vtype,capacity> hashM
//#define CreateComplexHashMap(hashM, ktype, vtype, capacity, comparator) HashMap<ktype,vtype,capacity> hashM(comparator)

//template<typename K, typename V, unsigned int capacity>
class HashMap
{
  public:
    //typedef bool (*comparator)(K, K);

    /*
    || @constructor
    || | Initialize this HashMap
    || #
    ||
    || @parameter compare optional function for comparing a key against another (for complex types)
    */
    HashMap()
    {
      //cb_comparator = compare;
      currentIndex = 0;
    }

    /*
    || @description
    || | Get the size of this HashMap
    || #
    ||
    || @return The size of this HashMap
    */
    unsigned int size() const
    {
      return currentIndex;
    }

    /*
    || @description
    || | Get a key at a specified index
    || #
    ||
    || @parameter idx the index to get the key at
    ||
    || @return The key at index idx
    */
    String keyAt(unsigned int idx)
    {
      return keys[idx];
    }

    /*
    || @description
    || | Get a value at a specified index
    || #
    ||
    || @parameter idx the index to get the value at
    ||
    || @return The value at index idx
    */
    String valueAt(unsigned int idx)
    {
      return values[idx];
    }

    /*
    || @description
    || | Check if a new assignment will overflow this HashMap
    || #
    ||
    || @return true if next assignment will overflow this HashMap
    */
    bool willOverflow()
    {
      return (currentIndex + 1 > capacity);
    }

    /*
    || @description
    || | An indexer for accessing and assigning a value to a key
    || | If a key is used that exists, it returns the value for that key
    || | If there exists no value for that key, the key is added
    || #
    ||
    || @parameter key the key to get the value for
    ||
    || @return The const value for key
    */
    //const String& operator[](const String key) const
    //{
    //  return operator[](key);
    //}

    /*
    || @description
    || | An indexer for accessing and assigning a value to a key
    || | If a key is used that exists, it returns the value for that key
    || | If there exists no value for that key, the key is added
    || #
    ||
    || @parameter key the key to get the value for
    ||
    || @return The value for key
    */
    /*
    String& operator[](const String key)
    {
      if (contains(key))
      {
        return values[indexOf(key)];
      }
      else if (currentIndex < capacity)
      {
        keys[currentIndex] = key;
        values[currentIndex] = nil;
        currentIndex++;
        //for(int i=0;i<currentIndex;i++) {
		//	Serial.print("KEEEEY: ");
		//	Serial.println(keys[i]);
		//}
        return values[currentIndex - 1];
      }
      return nil;
    }
    */
    
    bool addItemIfNotExist(String key, String value) {
		if(contains(key)) {
			if(getItem(key)=="") {
				addItem(key,value);
				return true;
			} else {
				return false;
			}
		} else {
			addItem(key,value);
			return true;
		};
	}
    
    void addItem(String key, String value) {
		if(key!="") {	
			if(contains(key)) {
				//Serial.print(value);
				//Serial.print("==");
				//Serial.println(" ");
				if(value.substring(0,1)=="!") {
					int index = indexOf(key);		  
					//Serial.println("REMOVE");
					//Serial.println(index);
					for (int i = index; i < capacity - 1; i++)
					{
						keys[i] = keys[i + 1];
						values[i] = values[i + 1];
					}
					currentIndex--;
				} else {
					values[indexOf(key)] = value;
				}
			} else if (currentIndex < capacity) {
				keys[currentIndex] = key;
				values[currentIndex] = value;
				currentIndex++;			
			}
		}	
	}
	
	String& getItem(String key) {
		if(contains(key)) {
			return values[indexOf(key)];
		} else {
			return nil;
		}
	}

    /*
    || @description
    || | Get the index of a key
    || #
    ||
    || @parameter key the key to get the index for
    ||
    || @return The index of the key, or -1 if key does not exist
    */
    unsigned int indexOf(String key)
    {
      for (int i = 0; i < currentIndex; i++)
      {
          if (key==keys[i])
          {
            return i;
          }
      }
      return -1;
    }

    /*
    || @description
    || | Check if a key is contained within this HashMap
    || #
    ||
    || @parameter key the key to check if is contained within this HashMap
    ||
    || @return true if it is contained in this HashMap
    */
    bool contains(String key)
    {
      for (int i = 0; i < currentIndex; i++)
      {
		  //Serial.print(key);
		  //Serial.print(" == ");
		  //Serial.print(keys[i]);
		  //Serial.print(" --- ");
		  //Serial.println(key.compareTo(keys[i]));
          if (key==keys[i])
          {
			//Serial.println(" YES");
            return true;
          } else {
			//Serial.println(" NO");		  
		  }
      }
      //Serial.println(" DONE");
      return false;
    }

    /*
    || @description
    || | Check if a key is contained within this HashMap
    || #
    ||
    || @parameter key the key to remove from this HashMap
    */
    void remove(String key)
    {
      //Serial.print("KEY: ");
      //Serial.print("<");
      //Serial.print(key);
      //Serial.println(">");
      if (contains(key))
      {
        int index = indexOf(key);		  
		//Serial.println("REMOVE");
        for (int i = index; i < capacity - 1; i++)
        {
          keys[i] = keys[i + 1];
          values[i] = values[i + 1];
        }
        currentIndex--;
      }
    }
    
    //THIS IS ADDED BY FABIO TO INITIALIZE TO ZERO
    void resetMap()
    {
		currentIndex = 0;
	}

    void setNullValue(String nullv)
    {
      nil = nullv;
    }

  protected:
    int capacity = 200;
    String keys[200];
    String values[200];
    String nil;
    int currentIndex;
};

#endif
// HASHMAP_H
