using UnityEngine;
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
    public float frictionCast;
    float rotationY = 0F;
    void Update()
    {
        RaycastHit hit;

        if (Physics.Raycast(transform.position, new Vector3(0, -1, 0), out hit, frictionCast))
        {
            Debug.DrawLine(transform.position, hit.point);

            if (hit.collider.tag == "Block")
            {
                //Vector3 direction = new Vector3(0, 0, 0);
                //direction = hit.collider.transform.position - transform.position;
                //direction.y = 0;
                //transform.position += direction;
                //GetComponent<Rigidbody>().AddForce(direction*100);
                transform.parent = hit.collider.transform;
            }
        }

        
        

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

        if (Input.GetKey(KeyCode.W))
        {
            transform.position += new Vector3(transform.forward.normalized.x + transform.up.normalized.x, 0, transform.forward.normalized.z + transform.up.normalized.z) * movementSpeed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            transform.position -= transform.right.normalized * movementSpeed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            transform.position -= new Vector3(transform.forward.normalized.x + transform.up.normalized.x, 0, transform.forward.normalized.z + transform.up.normalized.z) * movementSpeed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            transform.position += transform.right.normalized * movementSpeed;
        }
    }

    void Start()
    {
        GetComponent<CameraMovement>().enabled = false;
        // Make the rigid body not change rotation
        if (GetComponent<Rigidbody>())
        {
            GetComponent<Rigidbody>().useGravity = true;
            GetComponent<Rigidbody>().freezeRotation = true;
        }
    }
}
