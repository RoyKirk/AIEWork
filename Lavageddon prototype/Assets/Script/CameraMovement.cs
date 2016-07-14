using UnityEngine;
using System.Collections;

public class CameraMovement : MonoBehaviour {

    public float movementSpeed;
    public enum RotationAxes { MouseXAndY = 0, MouseX = 1, MouseY = 2 }
    public RotationAxes axes = RotationAxes.MouseXAndY;
    public float sensitivityX = 15F;
    public float sensitivityY = 15F;
    public float minimumX = -360F;
    public float maximumX = 360F;
    public float minimumY = -60F;
    public float maximumY = 60F;
    float rotationY = 0F;
    void Update()
    {
        if (Input.GetButton("Fire2"))
        {
            if (axes == RotationAxes.MouseXAndY)
            {
                if (Input.GetAxis("Mouse Y") != 0 || Input.GetAxis("Mouse X") != 0)
                {
                    Cursor.visible = true;
                    Cursor.lockState = CursorLockMode.None;

                    float rotationX = transform.localEulerAngles.y + Input.GetAxis("Mouse X") * sensitivityX;

                    rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
                    rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

                    transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
                }
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
        }



        if (axes == RotationAxes.MouseXAndY)
        {
            if (Input.GetAxis("Joy Y") != 0 || Input.GetAxis("Joy X") != 0)
            {
                Cursor.visible = false;
                Cursor.lockState = CursorLockMode.Locked;

                float rotationX = transform.localEulerAngles.y + Input.GetAxis("Joy X");

                rotationY += Input.GetAxis("Joy Y");
                rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

                transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
            }
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


        transform.position += Input.GetAxis("Vertical") * transform.forward.normalized * movementSpeed;

        transform.position += Input.GetAxis("Horizontal") * transform.right.normalized * movementSpeed;

        transform.position += Input.GetAxis("UpDown") * transform.up.normalized * movementSpeed;

        if (Input.GetButtonDown("Submit"))
        {
            GetComponent<PlayerMovement>().enabled = true;
        }
    }

    void Start()
    {
        Cursor.visible = true;
        // Make the rigid body not change rotation
        if (GetComponent<Rigidbody>())
        {
            GetComponent<Rigidbody>().freezeRotation = true;
            GetComponent<Rigidbody>().isKinematic = true;
        }

    }

}
