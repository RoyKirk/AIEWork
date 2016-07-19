using UnityEngine;
using System.Collections;

public class FiringScript : MonoBehaviour
{
    public GameObject projectile;
    public GameObject FiringPoint;

    public float speed = 100;

	// Use this for initialization
	void Start ()
    {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
	    
	}

    public void Fire()
    {
        GameObject bullet = Instantiate(projectile, FiringPoint.transform.position, FiringPoint.transform.rotation) as GameObject;
        Rigidbody rb = bullet.GetComponent<Rigidbody>();
        rb.AddRelativeForce(Vector3.forward * speed);
    }

    public void FireNetwork()
    {

    }
}
