﻿using UnityEngine;
using System.Collections;

public class PlayerMovement : MonoBehaviour {

    public float movementSpeed;
    public enum RotationAxes { MouseXAndY = 0, MouseX = 1, MouseY = 2 }
    public RotationAxes axes = RotationAxes.MouseXAndY;
    public float sensitivityX = 15F;
    public float sensitivityY = 15F;
    public float minimumX = -360F;
    public float maximumX = 360F;
    public float minimumY = -60F;
    public float maximumY = 60F;
    public float frictionCast = 1.0f;
    public float jumpForce = 1000.0f;
    float rotationY = 0F;

    Transform platform;

    Vector3 localPos;

    //public float shootDistance = 1000.0f;
    void Update()
    {

        RaycastHit hit;

        if (Physics.Raycast(transform.position, new Vector3(0, -1, 0), out hit, frictionCast))
        {

            Debug.DrawLine(transform.position, hit.point);
            if (Input.GetButtonDown("Jump"))
            {
                GetComponent<Rigidbody>().AddForce(0, jumpForce, 0);
            }
        }

        //controller look
        if (axes == RotationAxes.MouseXAndY)
        {
            float rotationX = transform.localEulerAngles.y + Input.GetAxis("Joy X");

            rotationY += Input.GetAxis("Joy Y");
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
        }
        else if (axes == RotationAxes.MouseX)
        {
            transform.Rotate(0, Input.GetAxis("Joy X"), 0);
        }
        else
        {
            rotationY += Input.GetAxis("Joy Y");
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
        }
        //mouse look
        if (axes == RotationAxes.MouseXAndY)
        {
            float rotationX = transform.localEulerAngles.y + Input.GetAxis("Mouse X") * sensitivityX;

            rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
        }
        else if (axes == RotationAxes.MouseX)
        {
            transform.Rotate(0, Input.GetAxis("Mouse X") * sensitivityX, 0);
        }
        else
        {
            rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
        }
        

        localPos += Input.GetAxis("Vertical") * new Vector3(transform.forward.normalized.x + transform.up.normalized.x, 0, transform.forward.normalized.z + transform.up.normalized.z) * movementSpeed;
        
        localPos += Input.GetAxis("Horizontal") * transform.right.normalized * movementSpeed;

        if (Input.GetButtonDown("Fire"))
        {
            GetComponent<FiringScript>().Fire();
            //RaycastHit shot;
            //if (Physics.Raycast(transform.position, transform.forward, out shot))
            //{
            //    Debug.DrawLine(transform.position, shot.point);
            //    if (shot.collider.tag == "Block")
            //    {

            //        shot.collider.GetComponent<BlockDamage>().Damage(bulletDamage);

            //    }
            //}
        }

    }

    void LateUpdate()
    {
        RaycastHit hit;

        if (Physics.Raycast(transform.position, new Vector3(0, -1, 0), out hit, frictionCast))
        {
            Debug.DrawLine(transform.position, hit.point);
            if (hit.collider.tag == "Block")
            {
                if (hit.collider.transform != platform)
                {
                    platform = hit.collider.transform;
                    localPos = hit.collider.transform.position - localPos;
                }
               // transform.position -= posDif;

                //Vector3 posDif = hit.collider.transform.position - transform.position;
                //posDif = new Vector3(posDif.x, posDif.y, posDif.z);
                //transform.position = hit.collider.transform.position - posDif;
            }
        }
        else
        {
            if (platform)
            {
                localPos = platform.position - localPos;
                platform = null;
            }
        }


        transform.position = localPos;


    }

    void Start()
    {
        //Cursor.visible = false;
        GetComponent<CameraMovement>().enabled = false;
        // Make the rigid body not change rotation
        if (GetComponent<Rigidbody>())
        {
            GetComponent<Rigidbody>().useGravity = true;
            GetComponent<Rigidbody>().freezeRotation = true;
            GetComponent<Rigidbody>().isKinematic = false;
        }
        localPos = transform.position;
        platform = null;

    }
}
