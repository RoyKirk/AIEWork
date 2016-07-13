using UnityEngine;
using System.Collections;


public class managerscript : MonoBehaviour {

    public enum BlockType
    {
        FLOAT,
        ARMOUR,
    };
    int numberOfBlockTypes = 2;
    public bool blockdestroyed = false;
    public float blockOffset;
    public GameObject blockPlacePrefabFloat;
    public GameObject blockPlacePrefabArmour;
    public GameObject blockPrefabFloat;
    public GameObject blockPrefabArmour;
    GameObject block;
    public float placementReach;
    public float placementOffset;
    public bool constructionMode;
    public BlockType blockType = BlockType.FLOAT;
    bool startConstruction = true;
    public float startDistance = 10;
    // Use this for initialization
    void Start ()
    {
        if (blockType == BlockType.FLOAT)
        {
            Instantiate(blockPrefabFloat, transform.position + transform.forward * startDistance, new Quaternion(0, 0, 0, 0));
        }
        if (blockType == BlockType.ARMOUR)
        {
            Instantiate(blockPrefabArmour, transform.position + transform.forward * startDistance, new Quaternion(0, 0, 0, 0));
        }
    }

    // Update is called once per frame
    void Update ()
    {
	    if(blockdestroyed)
        {
            FixedJoint[] joints = FindObjectsOfType(typeof(FixedJoint)) as FixedJoint[];
            foreach (FixedJoint joint in joints)
            {
                if (!joint.connectedBody)
                {
                    Destroy(joint);
                }
            }
            blockdestroyed = false;
        }

        if (constructionMode)
        {

            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;

            if (Physics.Raycast(ray, out hit, placementReach))
            {

                Debug.DrawLine(ray.origin, hit.point);

                if (hit.collider.tag == "Block")
                {
                    if (startConstruction)
                    {
                        if (blockType == BlockType.FLOAT)
                        {
                            block = (GameObject)Instantiate(blockPlacePrefabFloat, hit.collider.transform.position, hit.collider.transform.rotation);
                        }
                        if (blockType == BlockType.ARMOUR)
                        {
                            block = (GameObject)Instantiate(blockPlacePrefabArmour, hit.collider.transform.position, hit.collider.transform.rotation);
                        }
                        startConstruction = false;
                    }

                    block.transform.rotation = hit.collider.transform.rotation;
                    block.transform.position = hit.collider.transform.position + hit.normal.normalized*placementOffset;

                }

            }
            else
            {
                Destroy(block);
                startConstruction = true;
            }

            if (Input.GetButtonDown("Fire1") && block && block.GetComponent<PlacementBlockScript>().placeable)
            {
                if (blockType == BlockType.FLOAT)
                {
                    Instantiate(blockPrefabFloat, block.transform.position, block.transform.rotation);
                }
                if (blockType == BlockType.ARMOUR)
                {
                    Instantiate(blockPrefabArmour, block.transform.position, block.transform.rotation);
                }
            }

            if (Input.GetKeyUp(KeyCode.Space) && block)
            {
                blockType++;
                if((int)blockType == numberOfBlockTypes)
                {
                    blockType = (BlockType)0;
                }
                Destroy(block);
                if (blockType == BlockType.FLOAT)
                {
                    block = (GameObject)Instantiate(blockPlacePrefabFloat, block.transform.position, block.transform.rotation);
                }
                if (blockType == BlockType.ARMOUR)
                {
                    block = (GameObject)Instantiate(blockPlacePrefabArmour, block.transform.position, block.transform.rotation);
                }
            }
        }
        else
        {
            Destroy(block);
        }
        //if (!block.GetComponent<buildingblock>().placed)
        //{
        //    block.transform.position = transform.position + transform.forward.normalized * blockOffset;
        //    block.transform.rotation = transform.rotation;
        //}
        //if (block.GetComponent<buildingblock>().placed)
        //{
        //    block.GetComponent<BoxCollider>().enabled = true;
        //    block = (GameObject)Instantiate(blockPrefab, transform.position + transform.forward.normalized * blockOffset, transform.rotation);
        //    block.GetComponent<BoxCollider>().enabled = false;
        //}
    }
}
