using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;
public class PlayerMovement : MonoBehaviour {

    public enum Weapon
    {
        BOMB,
        LASER,
    };
    int numberOfWeapons = 2;
    public Weapon weapon = Weapon.BOMB;
    public int laserDamage = 1;
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

    public Color c1 = Color.yellow;
    public Color c2 = Color.red;
    int lengthOfLineRenderer = 10;

    public float laserMinTime = 0.02f;
    float laserTimer = 0.0f;


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
            //if (hit.collider.tag == "Block")
            //{

            //}
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

        laserTimer += Time.deltaTime;

        if (laserTimer >= laserMinTime)
        {
            LineRenderer lineRenderer = GetComponent<LineRenderer>();
            lineRenderer.enabled = false;
        }

        if (laserTimer < laserMinTime)
        {
            LineRenderer lineRenderer = GetComponent<LineRenderer>();
            int i = 0;
            while (i < lengthOfLineRenderer)
            {
                //Vector3 pos = (transform.position + transform.right.normalized*(lengthOfLineRenderer-i+1)/20) + (transform.forward.normalized * i - transform.right.normalized / (lengthOfLineRenderer - i + 1) / 20);
                //Vector3 pos = transform.position + transform.forward.normalized * i;
                Vector3 pos = (transform.position - transform.up.normalized * (lengthOfLineRenderer - i + 1) / 20) + (transform.forward.normalized * i + transform.up.normalized / (lengthOfLineRenderer - i + 1) / 20);
                lineRenderer.SetPosition(i, pos);
                i++;
            }
        }

        if (Input.GetButtonDown("Fire"))
        {

            if (weapon == Weapon.BOMB)
            {
                GetComponent<FiringScript>().Fire();
            }
            if (weapon == Weapon.LASER && laserTimer >= laserMinTime)
            {
                laserTimer = 0.0f;
                LineRenderer lineRenderer = GetComponent<LineRenderer>();
                lineRenderer.enabled = true;
                int i = 0;
                while (i < lengthOfLineRenderer)
                {
                    //Vector3 pos = (transform.position + transform.right.normalized*(lengthOfLineRenderer-i+1)/20) + (transform.forward.normalized * i - transform.right.normalized / (lengthOfLineRenderer - i + 1) / 20);
                    //Vector3 pos = transform.position + transform.forward.normalized * i;
                    Vector3 pos = (transform.position - transform.up.normalized * (lengthOfLineRenderer - i + 1) / 20) + (transform.forward.normalized * i + transform.up.normalized / (lengthOfLineRenderer - i + 1) / 20);
                   
                    lineRenderer.SetPosition(i, pos);
                    i++;
                }
                //laser.enabled = true;
                //int i = 0;
                //while (i < lengthOfLineRenderer)
                //{
                //    //Vector3 pos = new Vector3(i * 0.5F, Mathf.Sin(i + t), 0);
                //    Vector3 pos = transform.position + transform.forward.normalized * i;
                //    i++;
                //}
                //laser.SetPosition(0, transform.position);
                //laser.SetPosition(1, transform.position + transform.forward.normalized * laserLength);
                RaycastHit shot;
                if (Physics.Raycast(transform.position, transform.forward, out shot))
                {
                    Debug.DrawLine(transform.position, shot.point, Color.red);
                    if (shot.collider.tag == "Block")
                    {

                        shot.collider.GetComponent<BlockDamage>().Damage(laserDamage);

                    }
                }
            }
        }

        if (Input.GetButtonDown("SwitchWeapon"))
        {
            if (Input.GetAxis("SwitchWeapon") > 0)
            {
                weapon++;
            }
            if (Input.GetAxis("SwitchWeapon") < 0)
            {
                weapon--;
            }

            if ((int)weapon == numberOfWeapons)
            {
                weapon = (Weapon)0;
            }
            else if ((int)weapon < 0)
            {
                weapon = (Weapon)(numberOfWeapons - 1);
            }

        }
        else if (!Input.GetButton("SwitchWeapon") && !Input.GetButtonDown("SwitchWeapon") && Input.GetAxis("SwitchWeapon") != 0)
        {
            if (Input.GetAxis("SwitchWeapon") > 0)
            {
                weapon++;
            }
            if (Input.GetAxis("SwitchWeapon") < 0)
            {
                weapon--;
            }

            if ((int)weapon == numberOfWeapons)
            {
                weapon = (Weapon)0;
            }
            else if ((int)weapon < 0)
            {
                weapon = (Weapon)(numberOfWeapons - 1);
            }
        }
        if (Input.GetButtonDown("StartGame"))
        {
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);

        }

    }

    void LateUpdate()
    {
        RaycastHit hit;

        if (Physics.Raycast(transform.position, new Vector3(0, -1, 0), out hit, frictionCast))
        {
            if (hit.collider.tag == "Block")
            {
                Vector3 vel = hit.collider.GetComponent<Rigidbody>().velocity;
                GetComponent<Rigidbody>().velocity = new Vector3(vel.x, GetComponent<Rigidbody>().velocity.y, vel.z);
            }
            else
            {
                GetComponent<Rigidbody>().velocity = new Vector3(0, GetComponent<Rigidbody>().velocity.y, 0);
            }
        }
        else
        {
            GetComponent<Rigidbody>().velocity = new Vector3(0, GetComponent<Rigidbody>().velocity.y, 0);
        }
    }

    void Start()
    {
        //Cursor.visible = false;
        GetComponent<CameraMovement>().enabled = false;
        GetComponent<WhirlpoolCurrent>().enabled = true;
        // Make the rigid body not change rotation
        if (GetComponent<Rigidbody>())
        {
            GetComponent<Rigidbody>().useGravity = true;
            GetComponent<Rigidbody>().freezeRotation = true;
            GetComponent<Rigidbody>().isKinematic = false;
        }

        LineRenderer lineRenderer = gameObject.AddComponent<LineRenderer>();
        //LineRenderer lineRenderer = gameObject.GetComponent<LineRenderer>();
        //lineRenderer.enabled = true;
        lineRenderer.material = new Material(Shader.Find("Particles/Additive"));
        lineRenderer.SetColors(c1, c2);
        lineRenderer.SetWidth(0.2F, 0.2F);
        lineRenderer.SetVertexCount(lengthOfLineRenderer);
        lineRenderer.enabled = false;
    }
}
