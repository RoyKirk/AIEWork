using UnityEngine;
using UnityEngine.UI;
using System.Collections;


public class managerscript : MonoBehaviour {

    public enum BlockType
    {
        FLOAT,
        ARMOUR,
    };
    int numberOfBlockTypes = 2;
    public bool blockdestroyed = false;
    public GameObject blockPlacePrefabFloat;
    public GameObject blockPlacePrefabArmour;
    public GameObject blockPrefabFloat;
    public GameObject blockPrefabArmour;
    GameObject block;
    public float placementReach = 1000f;
    public float placementOffset = 1.2f;
    public bool constructionMode = true;
    public BlockType blockType = BlockType.FLOAT;
    bool startConstruction = true;
    public float startDistance = 10;
    public int maxNumberOfBlocks = 25;
    public int numberOfBlocks = 0;
    public Text numberText;
    // Use this for initialization
    void Start ()
    {
        if (blockType == BlockType.FLOAT)
        {
            GameObject temp = (GameObject)Instantiate(blockPrefabFloat, transform.position + transform.forward * startDistance, new Quaternion(0, 0, 0, 0));
            temp.GetComponent<BlockDamage>().keystone = true;
        }
        if (blockType == BlockType.ARMOUR)
        {
            GameObject temp = (GameObject)Instantiate(blockPrefabArmour, transform.position + transform.forward * startDistance, new Quaternion(0, 0, 0, 0));
            temp.GetComponent<BlockDamage>().keystone = true;
        }
    }

    // Update is called once per frame
    void Update ()
    {
        numberText.text = "No. of Blocks = " + (maxNumberOfBlocks - numberOfBlocks);

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

        if(Input.GetButtonDown("Exit"))
        {
            #if UNITY_EDITOR
                        UnityEditor.EditorApplication.isPlaying = false;
            #elif UNITY_WEBPLAYER
                     Application.OpenURL(webplayerQuitURL);
            #else
                     Application.Quit();
            #endif
        }

        if (constructionMode)
        {

            //Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;

            //if (Physics.Raycast(ray, out hit, placementReach))
            if (Physics.Raycast(transform.position, transform.forward, out hit, placementReach))
            {

                //Debug.DrawLine(ray.origin, hit.point);
                Debug.DrawLine(transform.position, hit.point);

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
                else if (hit.collider.tag != "Block" && hit.collider.tag != "PlaceBlock")
                {
                    Destroy(block);
                    startConstruction = true;
                }



            }
            ////controller without cursor set
            //else if (Physics.Raycast(transform.position, transform.forward, out hit, placementReach))
            //{

            //    Debug.DrawLine(ray.origin, hit.point);

            //    if (hit.collider.tag == "Block")
            //    {
            //        if (startConstruction)
            //        {
            //            if (blockType == BlockType.FLOAT)
            //            {
            //                block = (GameObject)Instantiate(blockPlacePrefabFloat, hit.collider.transform.position, hit.collider.transform.rotation);
            //            }
            //            if (blockType == BlockType.ARMOUR)
            //            {
            //                block = (GameObject)Instantiate(blockPlacePrefabArmour, hit.collider.transform.position, hit.collider.transform.rotation);
            //            }
            //            startConstruction = false;
            //        }

            //        block.transform.rotation = hit.collider.transform.rotation;
            //        block.transform.position = hit.collider.transform.position + hit.normal.normalized * placementOffset;

            //    }

            //}
            else
            {
                Destroy(block);
                startConstruction = true;
            }


            if (Input.GetButtonDown("PlaceBlock") && block && block.GetComponent<PlacementBlockScript>().placeable && numberOfBlocks<maxNumberOfBlocks)
            {
                if (blockType == BlockType.FLOAT)
                {
                    Instantiate(blockPrefabFloat, block.transform.position, block.transform.rotation);
                }
                if (blockType == BlockType.ARMOUR)
                {
                    Instantiate(blockPrefabArmour, block.transform.position, block.transform.rotation);
                }
                numberOfBlocks++;
            }

            if (Input.GetButtonDown("DestroyBlock"))
            {
                RaycastHit shot;
                if (Physics.Raycast(transform.position, transform.forward, out shot))
                {
                    Debug.DrawLine(transform.position, shot.point);
                    if (shot.collider.tag == "Block")
                    {
                        if (!shot.collider.GetComponent<BlockDamage>().keystone)
                        {
                            shot.collider.GetComponent<BlockDamage>().Damage(shot.collider.GetComponent<BlockDamage>().HitPoints);
                            numberOfBlocks--;
                        }
                    }
                }
            }

            if (Input.GetButtonDown("SwitchBlock"))
            {
                if (Input.GetAxis("SwitchBlock") > 0)
                {
                    blockType++;
                }
                if (Input.GetAxis("SwitchBlock") < 0)
                {
                    blockType--;
                }

                if((int)blockType == numberOfBlockTypes)
                {
                    blockType = (BlockType)0;
                }
                else if((int)blockType < 0)
                {
                    blockType = (BlockType)(numberOfBlockTypes -1);
                }

                if (block)
                {
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
            else if (!Input.GetButton("SwitchBlock") && !Input.GetButtonDown("SwitchBlock") && Input.GetAxis("SwitchBlock") != 0)
            {
                if (Input.GetAxis("SwitchBlock") > 0)
                {
                    blockType++;
                }
                if (Input.GetAxis("SwitchBlock") < 0)
                {
                    blockType--;
                }

                if ((int)blockType == numberOfBlockTypes)
                {
                    blockType = (BlockType)0;
                }
                else if ((int)blockType < 0)
                {
                    blockType = (BlockType)(numberOfBlockTypes - 1);
                }


                if (block)
                {
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
