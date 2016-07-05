using UnityEngine;
using System.Collections;

public class managerscript : MonoBehaviour {

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
    public bool FLOAT;
    bool startConstruction = true;
    public float startDistance = 10;
    // Use this for initialization
    void Start ()
    {
        if (FLOAT)
        {
            Instantiate(blockPrefabFloat, transform.position + transform.forward * startDistance, new Quaternion(0,0,0,0));
        }
        if (!FLOAT)
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
                        if (FLOAT)
                        {
                            block = (GameObject)Instantiate(blockPlacePrefabFloat, hit.collider.transform.position, hit.collider.transform.rotation);
                        }
                        if (!FLOAT)
                        {
                            block = (GameObject)Instantiate(blockPlacePrefabArmour, hit.collider.transform.position, hit.collider.transform.rotation);
                        }
                        startConstruction = false;
                    }

                    //this needs to change from xyz to up right forward so it is independent of rotation
                    block.transform.rotation = hit.collider.transform.rotation;
                    Vector3 direction = hit.collider.transform.position - hit.point;
                    direction.Normalize();

                    if (Mathf.Abs(direction.x) > Mathf.Abs(direction.y) && Mathf.Abs(direction.x) > Mathf.Abs(direction.z))
                    {
                        if (direction.x > 0)
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x - placementOffset, hit.collider.transform.position.y, hit.collider.transform.position.z);
                        }
                        else
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x + placementOffset, hit.collider.transform.position.y, hit.collider.transform.position.z);
                        }
                    }
                    else if (Mathf.Abs(direction.y) > Mathf.Abs(direction.x) && Mathf.Abs(direction.y) > Mathf.Abs(direction.z))
                    {
                        if (direction.y > 0)
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x, hit.collider.transform.position.y - placementOffset, hit.collider.transform.position.z);
                        }
                        else
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x, hit.collider.transform.position.y + placementOffset, hit.collider.transform.position.z);
                        }
                    }
                    else if (Mathf.Abs(direction.z) > Mathf.Abs(direction.y) && Mathf.Abs(direction.z) > Mathf.Abs(direction.x))
                    {
                        if (direction.z > 0)
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x, hit.collider.transform.position.y, hit.collider.transform.position.z - placementOffset);
                        }
                        else
                        {
                            block.transform.position = new Vector3(hit.collider.transform.position.x, hit.collider.transform.position.y, hit.collider.transform.position.z + placementOffset);
                        }
                    }
                    // block.transform.position = hit.point;
                }
            }

            if (Input.GetMouseButtonUp(0) && block)
            {
                if (FLOAT)
                {
                    Instantiate(blockPrefabFloat, block.transform.position, block.transform.rotation);
                }
                if (!FLOAT)
                {
                    Instantiate(blockPrefabArmour, block.transform.position, block.transform.rotation);
                }
            }

            if (Input.GetKeyUp(KeyCode.Space) && block)
            {
                FLOAT = !FLOAT;
                Destroy(block);
                if (FLOAT)
                {
                    block = (GameObject)Instantiate(blockPlacePrefabFloat, block.transform.position, block.transform.rotation);
                }
                if(!FLOAT)
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
