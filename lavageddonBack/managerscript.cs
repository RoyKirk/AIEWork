using UnityEngine;
using System.Collections;

public class managerscript : MonoBehaviour {

    public bool blockdestroyed = false;
    public float blockOffset;
    public GameObject blockPlacePrefab;
    public GameObject blockPrefab;
    GameObject block;
    public float placementReach;
    public float placementOffset;
    // Use this for initialization
    void Start ()
    {
        block = (GameObject)Instantiate(blockPlacePrefab, transform.position + transform.forward.normalized * blockOffset, transform.rotation);
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
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, placementReach))
        { 
            Debug.DrawLine(ray.origin, hit.point);

            if (hit.collider.tag == "Block")
            {
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

        if (Input.GetKeyUp(KeyCode.Space))
        {
            Instantiate(blockPrefab, block.transform.position, block.transform.rotation);
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
