using UnityEngine;
using System.Collections;

public class ExplosionScript : MonoBehaviour
{
    public float explosiveForce = 100;
    float radius;
    float delay = 0.25f;

    // Use this for initialization
    void Start ()
    {
        radius = transform.localScale.x;
	}
	
	// Update is called once per frame
	void Update ()
    {
        delay -= Time.deltaTime;
        if(delay <= 0)
        {
            Destroy(gameObject);
        }
	}

    void OnTriggerEnter(Collider other)
    {
        if(other.tag == "Block")
        {
            Rigidbody rb = other.GetComponent<Rigidbody>();
            rb.AddExplosionForce(explosiveForce, transform.position, radius);
        }
    }
}
