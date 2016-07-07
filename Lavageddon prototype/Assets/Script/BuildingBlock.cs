using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class BuildingBlock : MonoBehaviour {


    public float blockCast;
    
    // Use this for initialization
    void Start ()
    {
        GetComponent<BoxCollider>().enabled = false;
        RaycastHit hit;

        if (Physics.Raycast(transform.position, transform.up, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);
            
            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        if (Physics.Raycast(transform.position, transform.up*-1.0f, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        if (Physics.Raycast(transform.position, transform.forward, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        if (Physics.Raycast(transform.position, transform.forward * -1.0f, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        if (Physics.Raycast(transform.position, transform.right, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        if (Physics.Raycast(transform.position, transform.right * -1.0f, out hit, blockCast))
        {
            //Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                FixedJoint temp = gameObject.AddComponent<FixedJoint>();
                temp.connectedBody = hit.collider.GetComponent<Rigidbody>();
            }
        }

        GetComponent<BoxCollider>().enabled = true;
        GetComponent<Rigidbody>().isKinematic = true;
    }

    // Update is called once per frame
    void Update() {
        if(Input.GetKeyUp(KeyCode.KeypadEnter))
        {
            GetComponent<Rigidbody>().isKinematic = false;
            GetComponent<WhirlpoolCurrent>().enabled = true;
            GetComponent<FloatFixed>().enabled = true;
        }

    }

    void OnDestroy()
    {
        //destruction
        if (GameObject.Find("Main Camera"))
        {
            GameObject.Find("Main Camera").GetComponent<managerscript>().blockdestroyed = true;
        }
    }
}
