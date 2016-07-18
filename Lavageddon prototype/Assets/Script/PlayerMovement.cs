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
    public float frictionCast = 1.0f;
    public float jumpForce = 1000.0f;
    float rotationY = 0F;
    //public float shootDistance = 1000.0f;
    public float bulletDamage = 1.0f;
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
            if (hit.collider.tag == "Block")
            {
                //Vector3 direction = new Vector3(0, 0, 0);
                //direction = hit.collider.transform.position - transform.position;
                //direction.y = 0;
                //transform.position += direction;
                //GetComponent<Rigidbody>().AddForce(direction*100);

                transform.parent = hit.collider.transform;
                //Vector3 temp = Vector3.MoveTowards(transform.position,hit.collider.transform.position, 0.1f);
                //transform.position = new Vector3(temp.x, transform.position.y, temp.z);
                //velocity = (current - previous) / Time.deltaTime;


                //transform.position += hit.collider.GetComponent<Rigidbody>().velocity;
                //transform.position = new Vector3(hit.collider.transform.position.x, transform.position.y, hit.collider.transform.position.z);
                //transform.eulerAngles = new Vector3(hit.collider.transform.eulerAngles.x, transform.eulerAngles.y, hit.collider.transform.eulerAngles.z);
                //transform.localPosition = new Vector3(hit.collider.transform.position.x, transform.position.y, hit.collider.transform.position.z);

                //transform.position = hit.collider.transform.position;
                //Vector3 moveDelta = hit.GetComponent<PlatformSpeed>().moveDelta;

                //this.transform.position += (Vector3)moveDelta;

                //transform.eulerAngles -= new Vector3(hit.collider.transform.eulerAngles.x, 0, hit.collider.transform.eulerAngles.z);


                //transform.localPosition = new Vector3(hit.collider.transform.position.x, transform.localPosition.y, hit.collider.transform.position.z);

                //Vector3 eulerTemp = transform.eulerAngles + new Vector3(0, hit.collider.transform.eulerAngles.y, 0);
                //transform.eulerAngles += new Vector3(0, hit.collider.transform.eulerAngles.y, 0);
                //transform.eulerAngles = new Vector3(transform.eulerAngles.x, hit.collider.transform.eulerAngles.y, transform.eulerAngles.z);
            }
        }
        else
        {
            transform.parent = null;
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
        

        transform.position += Input.GetAxis("Vertical") * new Vector3(transform.forward.normalized.x + transform.up.normalized.x, 0, transform.forward.normalized.z + transform.up.normalized.z) * movementSpeed;
        
        transform.position += Input.GetAxis("Horizontal") * transform.right.normalized * movementSpeed;

        if (Input.GetButtonDown("Fire"))
        {
            RaycastHit shot;
            if (Physics.Raycast(transform.position, transform.forward, out shot))
            {
                Debug.DrawLine(transform.position, shot.point);
                if (shot.collider.tag == "Block")
                {
                    shot.collider.GetComponent<BlockDamage>().Damage(bulletDamage);
                }
            }
        }

    }

    void LateUpdate()
    {
        if (transform.parent)
        {
            transform.eulerAngles = new Vector3(transform.eulerAngles.x, transform.eulerAngles.y, 0);
            //transform.localEulerAngles -= new Vector3(transform.parent.transform.eulerAngles.x, transform.parent.transform.eulerAngles.y, transform.parent.transform.eulerAngles.z);
        }

        //    RaycastHit hit;

        //    if (Physics.Raycast(transform.position, new Vector3(0, -1, 0), out hit, frictionCast))
        //    {
        //        Debug.DrawLine(transform.position, hit.point);
        //        if (hit.collider.tag == "Block")
        //        {
        //            //Vector3 direction = new Vector3(0, 0, 0);
        //            //direction = hit.collider.transform.position - transform.position;
        //            //direction.y = 0;
        //            //transform.position += direction;
        //            //GetComponent<Rigidbody>().AddForce(direction*100);

        //            transform.eulerAngles -= new Vector3(hit.collider.transform.eulerAngles.x, 0, hit.collider.transform.eulerAngles.z);
        //            //transform.parent = hit.collider.transform;
        //            //transform.position = new Vector3(hit.collider.transform.position.x, transform.position.y, hit.collider.transform.position.z);

        //            //Vector3 eulerTemp = transform.eulerAngles + new Vector3(0, hit.collider.transform.eulerAngles.y, 0);
        //            //transform.eulerAngles += new Vector3(0, hit.collider.transform.eulerAngles.y, 0);
        //            //transform.eulerAngles = new Vector3(transform.eulerAngles.x, hit.collider.transform.eulerAngles.y, transform.eulerAngles.z);
        //        }
        //    }

    }

        void Start()
    {
        Cursor.visible = false;
        GetComponent<CameraMovement>().enabled = false;
        // Make the rigid body not change rotation
        if (GetComponent<Rigidbody>())
        {
            GetComponent<Rigidbody>().useGravity = true;
            GetComponent<Rigidbody>().freezeRotation = true;
            GetComponent<Rigidbody>().isKinematic = false;
        }

    }
}
